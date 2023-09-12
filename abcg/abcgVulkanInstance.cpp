/**
 * @file abcgVulkanInstance.cpp
 * @brief Definition of abcg::VulkanInstance
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgVulkanInstance.hpp"

#include <span>

#include "abcgApplication.hpp"
#include "abcgException.hpp"
#include "abcgVulkan.hpp"

#if defined(ABCG_VULKAN_DEBUG_REPORT)

namespace {
VKAPI_ATTR VkBool32 VKAPI_CALL
debugReportCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                    VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
                    [[maybe_unused]] void *pUserData) {

  std::string messageTypeName{};
  switch (messageType) {
  case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
    messageTypeName = "general";
    break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
    messageTypeName = "validation";
    break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
    messageTypeName = "performance";
    break;
  default:
    break;
  }

  std::function applyColor{
      [](std::string_view str) { return std::string{str}; }};
  std::string messageSeverityName{};
  switch (messageSeverity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    messageSeverityName = "verbose";
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    messageSeverityName = "info";
    applyColor = [](std::string_view str) { return abcg::toBlueString(str); };
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    messageSeverityName = "warning";
    applyColor = [](std::string_view str) { return abcg::toYellowString(str); };
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    messageSeverityName = "error";
    applyColor = [](std::string_view str) { return abcg::toRedString(str); };
    break;
  default:
    break;
  }
  auto messageHeader{
      fmt::format("[vulkan {} {}]:", messageTypeName, messageSeverityName)};
  fmt::print(stderr, "{} {}\n", applyColor(messageHeader),
             pCallbackData->pMessage);
  return VK_FALSE;
}

[[nodiscard]] vk::DebugUtilsMessengerCreateInfoEXT
getDebugMessengerCreateInfo() {
  auto severityFlags{// vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                     // vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eError};
  auto typeFlags{vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                 vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance};
  return {.messageSeverity = severityFlags,
          .messageType = typeFlags,
          .pfnUserCallback = debugReportCallback};
}

void destroyDebugUtilsMessengerEXT(vk::Instance instance,
                                   vk::DebugUtilsMessengerEXT debugMessenger,
                                   vk::AllocationCallbacks const *pAllocator) {
  if (auto func{reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
          instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"))}) {
    if (pAllocator != nullptr) {
      auto allocator{static_cast<VkAllocationCallbacks>(*pAllocator)};
      func(instance, debugMessenger, &allocator);
    } else {
      func(instance, debugMessenger, nullptr);
    }
  }
}
} // namespace

#endif // ABCG_VULKAN_DEBUG_REPORT

namespace {
[[nodiscard]] std::vector<char const *>
checkLayersSupport(std::vector<char const *> const &layers) {
  std::vector<char const *> unsupportedLayers;
  auto supportedLayers{vk::enumerateInstanceLayerProperties()};

  for (auto const &layerName : layers) {
    auto const hasLayer{[&](auto supportedLayer) {
      std::span const nameSpan{supportedLayer.layerName};
      return std::string{nameSpan.data()} == layerName;
    }};
    if (std::ranges::none_of(supportedLayers, hasLayer)) {
      unsupportedLayers.push_back(layerName);
    }
  }

  return unsupportedLayers;
}

[[nodiscard]] std::vector<char const *>
checkExtensionsSupport(std::vector<char const *> const &extensions) {
  std::vector<char const *> unsupportedExtensions;
  auto supportedExtensions{vk::enumerateInstanceExtensionProperties()};

  for (auto const &extensionName : extensions) {
    auto const hasExtension{[&](auto supportedExtension) {
      std::span const nameSpan{supportedExtension.extensionName};
      return std::string{nameSpan.data()} == extensionName;
    }};
    if (std::ranges::none_of(supportedExtensions, hasExtension)) {
      unsupportedExtensions.push_back(extensionName);
    }
  }
  return unsupportedExtensions;
}
} // namespace

void abcg::VulkanInstance::create(std::vector<char const *> const &layers,
                                  std::vector<char const *> const &extensions,
                                  std::string_view applicationName) {
  // Load Vulkan loader
  if (volkInitialize() != VK_SUCCESS) {
    throw abcg::RuntimeError("Vulkan loader not found");
  }

  // Check extensions support
  if (auto unsupportedExtensions{checkExtensionsSupport(extensions)};
      !unsupportedExtensions.empty()) {
    std::string errorMessage{"Required extensions not supported:"};
    for (auto &unsupportedExtension : unsupportedExtensions) {
      errorMessage += ' ' + std::string{unsupportedExtension};
    }
    throw abcg::RuntimeError(errorMessage);
  }

  // Check layers support
  if (auto unsupportedLayers{checkLayersSupport(layers)};
      !unsupportedLayers.empty()) {
    std::string errorMessage{"Required layers not supported:"};
    for (auto &unsupportedLayer : unsupportedLayers) {
      errorMessage += ' ' + std::string{unsupportedLayer};
    }
    throw abcg::RuntimeError(errorMessage);
  }

#if defined(ABCG_VULKAN_DEBUG_REPORT)
  auto debugCreateInfo{getDebugMessengerCreateInfo()};
#endif

  // Create application info to be passed to instance create info
  vk::ApplicationInfo const applicationInfo{
      .pApplicationName = applicationName.data(),
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "ABCg",
      .engineVersion = VK_MAKE_VERSION(ABCG_VERSION_MAJOR, ABCG_VERSION_MINOR,
                                       ABCG_VERSION_PATCH),
      .apiVersion = VK_API_VERSION_1_1};

  // Create Vulkan instance
  m_instance = vk::createInstance({
#if defined(ABCG_VULKAN_DEBUG_REPORT)
    .pNext = &debugCreateInfo,
#endif
    .pApplicationInfo = &applicationInfo,
    .enabledLayerCount = gsl::narrow<uint32_t>(layers.size()),
    .ppEnabledLayerNames = layers.data(),
    .enabledExtensionCount = gsl::narrow<uint32_t>(extensions.size()),
    .ppEnabledExtensionNames = extensions.data()
  });

  // Load all required Vulkan entry points and extensions
  volkLoadInstanceOnly(m_instance);

#if defined(ABCG_VULKAN_DEBUG_REPORT)
  m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugCreateInfo);
#endif
}

void abcg::VulkanInstance::destroy() {
#if defined(ABCG_VULKAN_DEBUG_REPORT)
  destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif
  m_instance.destroy();
}

/**
 * @brief Conversion to vk::Instance.
 */
abcg::VulkanInstance::operator vk::Instance const &() const noexcept {
  return m_instance;
}
