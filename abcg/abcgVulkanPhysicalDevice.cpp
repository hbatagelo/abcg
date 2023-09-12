/**
 * @file abcgVulkanPhysicalDevice.cpp
 * @brief Definition of abcg::VulkanPhysicalDevice
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgVulkanPhysicalDevice.hpp"

#include <cppitertools/itertools.hpp>
#include <gsl/gsl>
#include <set>
#include <span>

#include "abcgException.hpp"
#include "abcgVulkanInstance.hpp"

void abcg::VulkanPhysicalDevice::create(
    VulkanInstance const &instance, vk::SurfaceKHR const &surfaceKHR,
    std::vector<char const *> const &extensions,
    vk::SampleCountFlagBits sampleCount, bool useSeparateTransferQueue) {
  m_instance = instance;
  m_surfaceKHR = surfaceKHR;

  auto const physicalDevices{
      static_cast<vk::Instance>(instance).enumeratePhysicalDevices()};
  if (physicalDevices.empty()) {
    throw abcg::RuntimeError("Failed to find a GPU with Vulkan support");
  }

  // If there are two or more GPUs, try selecting the first discrete GPU
  m_physicalDevice = physicalDevices.at(0);
  for (auto const &physicalDevice : physicalDevices) {
    if (isDeviceSuitable(extensions, useSeparateTransferQueue)) {
      m_physicalDevice = physicalDevice;
      break;
    }
  }

  m_sampleCount = std::min(sampleCount, getMaxUsableSampleCount());
}

void abcg::VulkanPhysicalDevice::destroy() {}

/**
 * @brief Finds a type of memory that meets the given requirements.
 *
 * @param memoryTypeBitsRequirement Bit field of memory types.
 * @param requiredProperties Required memory properties.
 *
 * @return Bit index of @a memoryTypeBitsRequirement that corresponds to the
 * memory type that includes all the @a requiredProperties, or `std::nullopt` if
 * the requirements could not be met.
 */
std::optional<uint32_t> abcg::VulkanPhysicalDevice::findMemoryType(
    uint32_t memoryTypeBitsRequirement,
    vk::MemoryPropertyFlags requiredProperties) const {
  auto const &memoryProperties{m_physicalDevice.getMemoryProperties()};

  for (auto &&[memoryIndex, memoryType] :
       iter::enumerate(memoryProperties.memoryTypes)) {
    uint32_t const memoryTypeBits{1U << memoryIndex};
    auto const isRequiredMemoryType{
        (memoryTypeBitsRequirement & memoryTypeBits) != 0U};
    auto const hasRequiredProperties{
        (memoryType.propertyFlags & requiredProperties) == requiredProperties};
    if (isRequiredMemoryType && hasRequiredProperties) {
      return gsl::narrow<uint32_t>(memoryIndex);
    }
  }

  return std::nullopt;
}

/**
 * @brief Conversion to vk::PhysicalDevice.
 */
abcg::VulkanPhysicalDevice::operator vk::PhysicalDevice const &()
    const noexcept {
  return m_physicalDevice;
}

/**
 * @brief Access to abcg::VulkanInstance.
 *
 * @return Vulkan instance associated with this physical device.
 */
abcg::VulkanInstance const &
abcg::VulkanPhysicalDevice::getInstance() const noexcept {
  return m_instance;
}

/**
 * @brief Access to queues families.
 *
 * @return Queues families available to this physical device.
 */
abcg::VulkanQueuesFamilies const &
abcg::VulkanPhysicalDevice::getQueuesFamilies() const noexcept {
  return m_queuesFamilies;
}

/**
 * @brief Access to the surface.
 *
 * @return Surface associated with this physical device.
 */
vk::SurfaceKHR const &
abcg::VulkanPhysicalDevice::getSurfaceKHR() const noexcept {
  return m_surfaceKHR;
}

/**
 * @brief Access to number of samples.
 *
 * @return Sample count associated with this physical device.
 */
vk::SampleCountFlagBits
abcg::VulkanPhysicalDevice::getSampleCount() const noexcept {
  return m_sampleCount;
}

void abcg::VulkanPhysicalDevice::checkQueueFamily(
    vk::QueueFamilyProperties const &properties, uint32_t queueFamilyIndex,
    bool useSeparateTransferQueue) {
  // Check for graphics queue
  if (!m_queuesFamilies.graphics.has_value() &&
      properties.queueFlags & vk::QueueFlagBits::eGraphics) {
    // Take the first index
    m_queuesFamilies.graphics = queueFamilyIndex;
  }

  // Check for compute queue
  if (properties.queueFlags & vk::QueueFlagBits::eCompute) {
    // Take the last index, so we can favor families that are not the same of
    // the graphics queue
    m_queuesFamilies.compute = queueFamilyIndex;
  }

  // Check for present queue
  if (!m_queuesFamilies.present.has_value() &&
      m_physicalDevice.getSurfaceSupportKHR(queueFamilyIndex, m_surfaceKHR) ==
          VK_TRUE) {
    // Take the first index with surface support
    m_queuesFamilies.present = queueFamilyIndex;
  }

  // Check for transfer queue
  if (properties.queueFlags & vk::QueueFlagBits::eTransfer) {
    if (useSeparateTransferQueue) {
      // Must have a unique index
      if (queueFamilyIndex !=
              m_queuesFamilies.graphics.value_or(VK_QUEUE_FAMILY_IGNORED) &&
          queueFamilyIndex !=
              m_queuesFamilies.compute.value_or(VK_QUEUE_FAMILY_IGNORED)) {
        m_queuesFamilies.transfer = queueFamilyIndex;
      }
    } else if (!m_queuesFamilies.transfer.has_value()) {
      m_queuesFamilies.transfer = queueFamilyIndex;
    }
  }
}

void abcg::VulkanPhysicalDevice::findQueueFamilies(
    bool useSeparateTransferQueue) {
  uint32_t queueFamilyIndex{};
  for (auto const &properties : m_physicalDevice.getQueueFamilyProperties()) {
    checkQueueFamily(properties, queueFamilyIndex, useSeparateTransferQueue);
    ++queueFamilyIndex;
  }

  if (!m_queuesFamilies.graphics.has_value() ||
      !m_queuesFamilies.present.has_value()) {
    throw abcg::RuntimeError(
        "Device does not have a graphics or present queue");
  }
  if (useSeparateTransferQueue && !m_queuesFamilies.transfer.has_value()) {
    throw abcg::RuntimeError("Device does not have a separate transfer queue");
  }
}

// From a given list of image formats, returns the first one supported by the
// physical device, or `std::nullopt` if none is supported.
std::optional<vk::Format> abcg::VulkanPhysicalDevice::getFirstSupportedFormat(
    std::vector<vk::Format> const &candidates, vk::ImageTiling tiling,
    vk::FormatFeatureFlags features) const {
  for (auto &&format : candidates) {
    auto const properties{m_physicalDevice.getFormatProperties(format)};

    if (tiling == vk::ImageTiling::eLinear &&
        (properties.linearTilingFeatures & features) == features) {
      return format;
    }
    if (tiling == vk::ImageTiling::eOptimal &&
        (properties.optimalTilingFeatures & features) == features) {
      return format;
    }
  }

  return std::nullopt;
}

vk::SampleCountFlagBits abcg::VulkanPhysicalDevice::getMaxUsableSampleCount() {
  auto const properties{m_physicalDevice.getProperties()};
  auto const countFlags{properties.limits.framebufferColorSampleCounts &
                        properties.limits.framebufferDepthSampleCounts};

  for (auto &&sampleCount :
       {vk::SampleCountFlagBits::e64, vk::SampleCountFlagBits::e32,
        vk::SampleCountFlagBits::e16, vk::SampleCountFlagBits::e8,
        vk::SampleCountFlagBits::e4, vk::SampleCountFlagBits::e2,
        vk::SampleCountFlagBits::e1}) {
    if (countFlags & sampleCount) {
      return sampleCount;
    }
  }

  return vk::SampleCountFlagBits::e1;
}

bool abcg::VulkanPhysicalDevice::isDeviceSuitable(
    std::vector<char const *> const &extensions,
    bool useSeparateTransferQueue) {
  auto swapchainIsAdequate{false};

  findQueueFamilies(useSeparateTransferQueue);
  auto const &queueFamilyAdequate{m_queuesFamilies.graphics.has_value() &&
                                  m_queuesFamilies.present.has_value()};

  auto const &extensionsSupported{checkExtensionsSupport(extensions).empty()};
  if (extensionsSupported) {
    swapchainIsAdequate =
        !m_physicalDevice.getSurfaceFormatsKHR(m_surfaceKHR).empty() &&
        !m_physicalDevice.getSurfacePresentModesKHR(m_surfaceKHR).empty();
  }

  auto const isDiscrete{vk::PhysicalDeviceType::eDiscreteGpu ==
                        m_physicalDevice.getProperties().deviceType};

  auto const &supportedFeatures{m_physicalDevice.getFeatures()};
  auto const hasSamplerAnisotropy{VK_TRUE ==
                                  supportedFeatures.samplerAnisotropy};

  return queueFamilyAdequate && extensionsSupported && swapchainIsAdequate &&
         isDiscrete && hasSamplerAnisotropy;
}

std::vector<char const *> abcg::VulkanPhysicalDevice::checkExtensionsSupport(
    std::vector<char const *> const &extensions) {
  std::vector<char const *> unsupportedExtensions;
  auto const supportedExtensions{
      m_physicalDevice.enumerateDeviceExtensionProperties()};

  std::set<std::string> requiredExtensions(extensions.begin(),
                                           extensions.end());

  for (auto const &extension : supportedExtensions) {
    requiredExtensions.erase(std::span{extension.extensionName}.data());
  }

  // Copy std::set<std::string> to std::vector<char const *>
  std::ranges::transform(requiredExtensions,
                         std::back_inserter(unsupportedExtensions),
                         [](std::string const &str) { return str.c_str(); });

  return unsupportedExtensions;
}