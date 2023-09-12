/**
 * @file abcgVulkanError.cpp
 * @brief Definition of abcg::VulkanError members.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgVulkanError.hpp"

#include <fmt/core.h>

#if !defined(NDEBUG)

void abcg::checkVkResult(VkResult retCode,
                         source_location const &sourceLocation) {
  if (retCode < 0)
    throw abcg::VulkanError(retCode, sourceLocation);

  // Success codes

  std::string_view result;
  switch (retCode) {
  case VK_SUCCESS:
    return;
  case VK_NOT_READY:
    result = "VK_NOT_READY: a fence or query has not yet completed";
    break;
  case VK_TIMEOUT:
    result =
        "VK_TIMEOUT: a wait operation has not completed in the specified time";
    break;
  case VK_EVENT_SET:
    result = "VK_EVENT_SET: an event is signaled";
    break;
  case VK_EVENT_RESET:
    result = "VK_EVENT_RESET: an event is unsignaled";
    break;
  case VK_INCOMPLETE:
    result = "a return array was too small for the result";
    break;
  case VK_SUBOPTIMAL_KHR:
    result = "VK_SUBOPTIMAL_KHR: a swapchain no longer matches the surface "
             "properties exactly, but can still be used to present to the "
             "surface successfully";
    break;
  case VK_THREAD_IDLE_KHR:
    result =
        "VK_THREAD_IDLE_KHR: a deferred operation is not complete but there is "
        "currently no work for this thread to do at the time of this call";
    break;
  case VK_THREAD_DONE_KHR:
    result = "VK_THREAD_DONE_KHR: a deferred operation is not complete but "
             "there is no work remaining to assign to additional threads";
    break;
  case VK_OPERATION_DEFERRED_KHR:
    result = "VK_OPERATION_DEFERRED_KHR: a deferred operation was requested "
             "and at least some of the work was deferred";
    break;
  case VK_OPERATION_NOT_DEFERRED_KHR:
    result = "VK_OPERATION_NOT_DEFERRED_KHR: a deferred operation was "
             "requested and no operations were deferred";
    break;
  case VK_PIPELINE_COMPILE_REQUIRED:
    result = "VK_PIPELINE_COMPILE_REQUIRED: a requested pipeline creation "
             "would have required compilation, but the application requested "
             "compilation to not be performed";
    break;
  default:
    result = fmt::format("Unknown success code {}", static_cast<int>(retCode));
    break;
  }

  fmt::print("{}: {}.\n", toYellowString("INFO"), result);
}

/**
 * @brief Constructs an abcg::VulkanError object.
 *
 * @param errorCode Vulkan return code.
 * @param sourceLocation Information about the source code.
 *
 * The object generates a color-coded string containing the error name, a
 * descriptive error messages, and the source code information.
 */
abcg::VulkanError::VulkanError(VkResult errorCode,
                               source_location const &sourceLocation)
    : Exception(prettyPrint(errorCode, sourceLocation)) {}

std::string
abcg::VulkanError::prettyPrint(VkResult errorCode,
                               source_location const &sourceLocation) {
  auto errorMessage{toRedString("Vulkan error ")};
  errorMessage += " (";
  errorMessage += getVulkanErrorString(errorCode);
  errorMessage += ")";
  return errorMessage + " in " + sourceLocation.file_name() + ":" +
         std::to_string(sourceLocation.line()) + ", " +
         toYellowString(sourceLocation.function_name()) + "\n";
}

#else
void abcg::checkVkResult([[maybe_unused]] VkResult retCode) {}

abcg::VulkanError::VulkanError(VkResult errorCode)
    : Exception(prettyPrint(errorCode)) {}
abcg::VulkanError::VulkanError(vk::Result /*errorCode*/) : Exception("TODO") {}

std::string abcg::VulkanError::prettyPrint(VkResult errorCode) {
  auto errorMessage{toRedString("Vulkan error ")};
  errorMessage += " (";
  errorMessage += getVulkanErrorString(errorCode);
  errorMessage += ")";
  return errorMessage;
}
#endif

// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkResult.html)
std::string_view abcg::VulkanError::getVulkanErrorString(VkResult errorCode) {
  std::string_view result;
  switch (errorCode) {
  case VK_ERROR_OUT_OF_HOST_MEMORY:
    result = "VK_ERROR_OUT_OF_HOST_MEMORY: a host memory allocation has failed";
    break;
  case VK_ERROR_OUT_OF_DEVICE_MEMORY:
    result =
        "VK_ERROR_OUT_OF_DEVICE_MEMORY: a device memory allocation has failed";
    break;
  case VK_ERROR_INITIALIZATION_FAILED:
    result = "VK_ERROR_INITIALIZATION_FAILED: initialization of an object "
             "could not be completed for implementation-specific reasons";
    break;
  case VK_ERROR_DEVICE_LOST:
    result =
        "VK_ERROR_DEVICE_LOST: the logical or physical device has been lost";
    break;
  case VK_ERROR_MEMORY_MAP_FAILED:
    result =
        "VK_ERROR_MEMORY_MAP_FAILED: mapping of a memory object has failed";
    break;
  case VK_ERROR_LAYER_NOT_PRESENT:
    result = "VK_ERROR_LAYER_NOT_PRESENT: a requested layer is not present or "
             "could not be loaded";
    break;
  case VK_ERROR_EXTENSION_NOT_PRESENT:
    result = "VK_ERROR_EXTENSION_NOT_PRESENT: a requested extension is not "
             "supported";
    break;
  case VK_ERROR_FEATURE_NOT_PRESENT:
    result =
        "VK_ERROR_FEATURE_NOT_PRESENT: a requested feature is not supported";
    break;
  case VK_ERROR_INCOMPATIBLE_DRIVER:
    result = "VK_ERROR_INCOMPATIBLE_DRIVER: the requested version of Vulkan is "
             "not supported by the driver or is otherwise incompatible for "
             "implementation-specific reasons";
    break;
  case VK_ERROR_TOO_MANY_OBJECTS:
    result = "VK_ERROR_TOO_MANY_OBJECTS: too many objects of the type have "
             "already been created";
    break;
  case VK_ERROR_FORMAT_NOT_SUPPORTED:
    result = "VK_ERROR_FORMAT_NOT_SUPPORTED: a requested format is not "
             "supported on this device";
    break;
  case VK_ERROR_FRAGMENTED_POOL:
    result = "VK_ERROR_FRAGMENTED_POOL: a pool allocation has failed due to "
             "fragmentation of the pool's memory. No attempt to allocate host "
             "or device memory was made to accommodate the new allocation";
    break;
  case VK_ERROR_SURFACE_LOST_KHR:
    result = "VK_ERROR_SURFACE_LOST_KHR: a surface is no longer available";
    break;
  case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
    result = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: the requested window is "
             "already in use by Vulkan or another API in a manner which "
             "prevents it from being used again";
    break;
  case VK_ERROR_OUT_OF_DATE_KHR:
    result =
        "VK_ERROR_OUT_OF_DATE_KHR: a surface has changed in such a way "
        "that it is no longer compatible with the swapchain, and further "
        "presentation requests using the swapchain will fail. Applications "
        "must query the new surface properties and recreate their swapchain if "
        "they wish to continue presenting to the surface";
    break;
  case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
    result = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: the display used by a "
             "swapchain does not use the same presentable image layout, or is "
             "incompatible in a way that prevents sharing an image";
    break;
  case VK_ERROR_VALIDATION_FAILED_EXT:
    result = "VK_ERROR_VALIDATION_FAILED_EXT: the validation layer has failed";
    break;
  case VK_ERROR_INVALID_SHADER_NV:
    result = "VK_ERROR_INVALID_SHADER_NV: one or more shaders failed to "
             "compile or link";
    break;
  case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
    result = "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: failed to "
             "satisfy all valid usage requirements for creating image";
    break;
  case VK_ERROR_NOT_PERMITTED_KHR:
    result = "VK_ERROR_NOT_PERMITTED_KHR (VK_KHR_global_priority)";
    break;
  case VK_ERROR_OUT_OF_POOL_MEMORY:
    result = "VK_ERROR_OUT_OF_POOL_MEMORY: a pool memory allocation has "
             "failed. No attempt to allocate host or device memory was made to "
             "accommodate the new allocation";
    break;
  case VK_ERROR_INVALID_EXTERNAL_HANDLE:
    result = "VK_ERROR_INVALID_EXTERNAL_HANDLE: an external handle is not a "
             "valid handle of the specified type";
    break;
  case VK_ERROR_FRAGMENTATION:
    result = "VK_ERROR_FRAGMENTATION: a descriptor pool creation has failed "
             "due to fragmentation";
    break;
  case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
    result = "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: a buffer creation or "
             "memory allocation failed because the requested address is not "
             "available. A shader group handle assignment failed because the "
             "requested shader group handle information is no longer valid";
    break;
  case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
    result = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: an operation on a "
             "swapchain created with "
             "VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it "
             "did not have exclusive full-screen access. This may occur due to "
             "implementation-dependent reasons, outside of the application's "
             "control";
    break;
  case VK_ERROR_UNKNOWN:
    result = "VK_ERROR_UNKNOWN: an unknown error has occurred; either the "
             "application has provided invalid input, or an implementation "
             "failure has occurred";
    break;
  default:
    result = fmt::format("Unknown error code {}", static_cast<int>(errorCode));
  }
  return result;
}
