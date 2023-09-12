/**
 * @file abcgVulkanDevice.hpp
 * @brief Header file of abcg::VulkanDevice
 *
 * Declaration of abcg::VulkanDevice.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_VULKAN_DEVICE_HPP_
#define ABCG_VULKAN_DEVICE_HPP_

#include "abcgVulkanPhysicalDevice.hpp"

#include <functional>

namespace abcg {
struct VulkanCommandPools;
struct VulkanQueues;
class VulkanDevice;
class VulkanPipeline;
class VulkanSwapchain;
class VulkanWindow;
} // namespace abcg

/**
 * @brief Command pools associated with a Vulkan device.
 */
struct abcg::VulkanCommandPools {
  vk::CommandPool compute;
  vk::CommandPool graphics;
  vk::CommandPool transfer;
};

/**
 * @brief Queues associated with a Vulkan device.
 */
struct abcg::VulkanQueues {
  vk::Queue compute;
  vk::Queue graphics;
  vk::Queue present;
  vk::Queue transfer;
};

/**
 * @brief A class for representing a Vulkan logical device and related
 * resources.
 *
 * This class creates and manages the Vulkan logical device, queues, descriptor
 * pool, and command pools.
 */
class abcg::VulkanDevice {
public:
  void create(VulkanPhysicalDevice const &physicalDevice,
              std::vector<char const *> const &extensions = {});
  void destroy();

  explicit operator vk::Device const &() const noexcept;

  [[nodiscard]] VulkanPhysicalDevice const &getPhysicalDevice() const noexcept;
  [[nodiscard]] VulkanQueues const &getQueues() const noexcept;
  [[nodiscard]] VulkanCommandPools const &getCommandPools() const noexcept;

  void withCommandBuffer(
      std::function<void(vk::CommandBuffer const &commandBuffer)> const &fun,
      vk::QueueFlagBits queueFlag = vk::QueueFlagBits::eGraphics,
      vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary) const;

private:
  void createCommandPools();
  void destroyCommandPools();

  vk::Device m_device;
  VulkanPhysicalDevice m_physicalDevice;
  VulkanCommandPools m_commandPools;
  VulkanQueues m_queues;
};

#endif