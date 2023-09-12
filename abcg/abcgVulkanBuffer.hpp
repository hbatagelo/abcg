/**
 * @file abcgVulkanBuffer.hpp
 * @brief Header file of abcg::VulkanBuffer
 *
 * Declaration of abcg::VulkanBuffer
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_VULKAN_BUFFER_HPP_
#define ABCG_VULKAN_BUFFER_HPP_

#include "abcgVulkanDevice.hpp"

#include <gsl/pointers>

namespace abcg {
struct VulkanBufferCreateInfo;
class VulkanBuffer;
} // namespace abcg

/**
 * @brief Creation info structure for abcg::VulkanBuffer::create.
 */
struct abcg::VulkanBufferCreateInfo {
  vk::DeviceSize size{};
  vk::BufferUsageFlags usage{};
  vk::MemoryPropertyFlags properties{};
  std::optional<gsl::not_null<void const *>> data{};
};

/**
 * @brief A class for representing a Vulkan buffer.
 *
 * This class provides helper functions for creating and managing vk::Buffer
 * objects.
 */
class abcg::VulkanBuffer {
public:
  void create(VulkanDevice const &device,
              VulkanBufferCreateInfo const &createInfo);
  void destroy();
  void loadData(gsl::not_null<void const *> data, vk::DeviceSize size,
                vk::DeviceSize offset = 0UL);

  explicit operator vk::Buffer const &() const noexcept;

  [[nodiscard]] vk::DeviceMemory const &getDeviceMemory() const noexcept;

private:
  [[nodiscard]] std::pair<vk::Buffer, vk::DeviceMemory>
  createBuffer(VulkanDevice const &device, vk::DeviceSize size,
               vk::BufferUsageFlags usage,
               vk::MemoryPropertyFlags properties) const;

  vk::Buffer m_buffer;
  vk::DeviceMemory m_deviceMemory;
  vk::Device m_device;
};

#endif