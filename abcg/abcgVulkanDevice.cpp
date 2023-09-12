/**
 * @file abcgVulkanDevice.cpp
 * @brief Definition of abcg::VulkanDevice
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgVulkanDevice.hpp"

#include <gsl/gsl>

#include <set>

void abcg::VulkanDevice::create(VulkanPhysicalDevice const &physicalDevice,
                                std::vector<char const *> const &extensions) {
  m_physicalDevice = physicalDevice;
  auto const &queuesFamilies{m_physicalDevice.getQueuesFamilies()};
  auto const graphicsQueueFamily{queuesFamilies.graphics.value_or(0)};
  auto const presentQueueFamily{queuesFamilies.present.value_or(0)};

  std::set uniqueQueueFamilies{graphicsQueueFamily, presentQueueFamily};
  if (queuesFamilies.compute.has_value()) {
    uniqueQueueFamilies.insert(queuesFamilies.compute.value());
  }
  if (queuesFamilies.transfer.has_value()) {
    uniqueQueueFamilies.insert(queuesFamilies.transfer.value());
  }

  auto queuePriority{1.0f};
  std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
  deviceQueueCreateInfos.reserve(uniqueQueueFamilies.size());
  for (auto queueFamily : uniqueQueueFamilies) {
    deviceQueueCreateInfos.push_back({.queueFamilyIndex = queueFamily,
                                      .queueCount = 1,
                                      .pQueuePriorities = &queuePriority});
  }

  auto deviceFeatures{
      static_cast<vk::PhysicalDevice>(m_physicalDevice).getFeatures()};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  m_device =
      static_cast<vk::PhysicalDevice>(m_physicalDevice)
          .createDevice({.queueCreateInfoCount = gsl::narrow<uint32_t>(
                             deviceQueueCreateInfos.size()),
                         .pQueueCreateInfos = deviceQueueCreateInfos.data(),
                         .enabledExtensionCount =
                             gsl::narrow<uint32_t>(extensions.size()),
                         .ppEnabledExtensionNames = extensions.data(),
                         .pEnabledFeatures = &deviceFeatures});

  // Load device-related entry points directly from the driver
  volkLoadDevice(m_device);

  m_queues.graphics = m_device.getQueue(graphicsQueueFamily, 0);
  m_queues.present = m_device.getQueue(presentQueueFamily, 0);

  if (queuesFamilies.compute.has_value()) {
    m_queues.compute = m_device.getQueue(queuesFamilies.compute.value(), 0);
  }
  if (queuesFamilies.transfer.has_value()) {
    m_queues.transfer = m_device.getQueue(queuesFamilies.transfer.value(), 0);
  }

  createCommandPools();
}

void abcg::VulkanDevice::destroy() {
  destroyCommandPools();
  m_device.destroy();
}

/**
 * @brief Conversion to vk::Device.
 */
abcg::VulkanDevice::operator vk::Device const &() const noexcept {
  return m_device;
}

/**
 * @brief Access to abcg::VulkanPhysicalDevice.
 *
 * @return Instance of vulkan physical device associated with this device.
 */
abcg::VulkanPhysicalDevice const &
abcg::VulkanDevice::getPhysicalDevice() const noexcept {
  return m_physicalDevice;
}

/**
 * @brief Returns the queues associated with this device.
 *
 * @return Queues structure.
 */
abcg::VulkanQueues const &abcg::VulkanDevice::getQueues() const noexcept {
  return m_queues;
}

/**
 * @brief Returns the command pools associated with this device.
 *
 * @return Command pools structure.
 */
abcg::VulkanCommandPools const &
abcg::VulkanDevice::getCommandPools() const noexcept {
  return m_commandPools;
}

/**
 * @brief Allocates and creates a command buffer to be immediately submitted and
 * released.
 *
 * @param fun Function to be called between the begin and end calls of the
 * command buffer.
 * @param queueFlag Which command pool queue will be used. The graphics queue
 * command pool is the default.
 * @param level Whether a primary (default) or secondary command buffer will be
 * created.
 */
void abcg::VulkanDevice::withCommandBuffer(
    std::function<void(vk::CommandBuffer const &commandBuffer)> const &fun,
    vk::QueueFlagBits queueFlag, vk::CommandBufferLevel level) const {
  vk::Queue const *queue{};
  vk::CommandPool const *commandPool{};

  // Initialize pointer to selected queue and corresponding command pool
  switch (queueFlag) {
  case vk::QueueFlagBits::eCompute:
    queue = &m_queues.compute;
    commandPool = &m_commandPools.compute;
    break;
  case vk::QueueFlagBits::eTransfer:
    queue = &m_queues.transfer;
    commandPool = &m_commandPools.transfer;
    break;
  case vk::QueueFlagBits::eGraphics:
  default:
    queue = &m_queues.graphics;
    commandPool = &m_commandPools.graphics;
  }

  // Allocate command buffer from selected pool
  auto commandBuffer{m_device
                         .allocateCommandBuffers({.commandPool = *commandPool,
                                                  .level = level,
                                                  .commandBufferCount = 1})
                         .front()};

  // Start recording
  commandBuffer.begin(
      {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
  fun(commandBuffer);
  commandBuffer.end();

  // Queue command buffer
  queue->submit({{.commandBufferCount = 1, .pCommandBuffers = &commandBuffer}},
                vk::Fence{});

  // Wait until completion
  queue->waitIdle();

  // Cleanup
  m_device.freeCommandBuffers(*commandPool, {commandBuffer});
}

void abcg::VulkanDevice::createCommandPools() {
  auto const &queuesFamilies{m_physicalDevice.getQueuesFamilies()};
  auto const graphicsQueueFamily{queuesFamilies.graphics.value_or(0)};

  // Always create a command pool for the graphics queue
  m_commandPools.graphics = m_device.createCommandPool(
      {.flags = vk::CommandPoolCreateFlagBits::eTransient,
       .queueFamilyIndex = graphicsQueueFamily});

  if (queuesFamilies.compute.has_value()) {
    if (queuesFamilies.compute.value() == graphicsQueueFamily) {
      // Reuse command pool from graphics queue
      m_commandPools.compute = m_commandPools.graphics;
    } else {
      // Create a command pool for the compute queue only if the compute queue
      // and graphics queue are not from the same family
      m_commandPools.compute = m_device.createCommandPool(
          {.flags = vk::CommandPoolCreateFlagBits::eTransient,
           .queueFamilyIndex = queuesFamilies.compute.value()});
    }
  }

  if (queuesFamilies.transfer.has_value()) {
    if (queuesFamilies.transfer.value() == graphicsQueueFamily) {
      // Reuse command pool from graphics queue
      m_commandPools.transfer = m_commandPools.graphics;
    } else {
      // Create a command pool for the transfer queue only if the transfer queue
      // and graphics queue are not from the same family
      m_commandPools.transfer = m_device.createCommandPool(
          {.flags = vk::CommandPoolCreateFlagBits::eTransient,
           .queueFamilyIndex = queuesFamilies.transfer.value()});
    }
  }
}

void abcg::VulkanDevice::destroyCommandPools() {
  if (m_commandPools.compute != m_commandPools.graphics) {
    m_device.destroyCommandPool(m_commandPools.compute);
  }

  if (m_commandPools.transfer != m_commandPools.graphics) {
    m_device.destroyCommandPool(m_commandPools.transfer);
  }

  m_device.destroyCommandPool(m_commandPools.graphics);
}
