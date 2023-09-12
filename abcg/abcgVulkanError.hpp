/**
 * @file abcgVulkanError.hpp
 * @brief Header file of abcg::VulkanError and derived classes.
 *
 * Declaration of abcg::VulkanError and derived classes.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_VULKAN_ERROR_HPP_
#define ABCG_VULKAN_ERROR_HPP_

#if !defined(NDEBUG)
#if defined(__clang__)
#include <experimental/source_location>
namespace abcg {
using source_location = std::experimental::source_location;
} // namespace abcg
#else
#include <source_location>
namespace abcg {
using source_location = std::source_location;
} // namespace abcg
#endif
#endif

#include <volk.h>
#include <vulkan/vulkan.hpp>

#include "abcgException.hpp"

namespace abcg {
class VulkanError;
#if !defined(NDEBUG)
void checkVkResult(VkResult retCode, source_location const &sourceLocation =
                                         source_location::current());
#else
void checkVkResult([[maybe_unused]] VkResult retCode);
#endif
} // namespace abcg

/**
 * @brief Represents an exception object for Vulkan runtime errors.
 *
 * This is used for throwing exceptions for Vulkan runtime errors.
 *
 * The explanatory error message is appended with source location information,
 * and descriptive messages regarding the return codes.
 */
class abcg::VulkanError : public abcg::Exception {
#if !defined(NDEBUG)
public:
  explicit VulkanError(
      VkResult errorCode,
      source_location const &sourceLocation = source_location::current());

private:
  static std::string prettyPrint(VkResult errorCode,
                                 source_location const &sourceLocation);
#else
public:
  explicit VulkanError(VkResult errorCode);
  explicit VulkanError(vk::Result errorCode);

private:
  static std::string prettyPrint(VkResult errorCode);
#endif

  static std::string_view getVulkanErrorString(VkResult errorCode);
};

#endif
