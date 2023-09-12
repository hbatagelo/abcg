/**
 * @file abcgVulkanInstance.hpp
 * @brief Header file of abcg::VulkanInstance
 *
 * Declaration of abcg::VulkanInstance.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_VULKAN_INSTANCE_HPP_
#define ABCG_VULKAN_INSTANCE_HPP_

#include "abcgVulkanExternal.hpp"

namespace abcg {
class VulkanInstance;
} // namespace abcg

/**
 * @brief A class for representing a Vulkan instance.
 */
class abcg::VulkanInstance {
public:
  void create(std::vector<char const *> const &layers = {},
              std::vector<char const *> const &extensions = {},
              std::string_view applicationName = {});
  void destroy();

  explicit operator vk::Instance const &() const noexcept;

private:
  vk::Instance m_instance;

#if defined(ABCG_VULKAN_DEBUG_REPORT)
  vk::DebugUtilsMessengerEXT m_debugMessenger{};
#endif
};

#endif