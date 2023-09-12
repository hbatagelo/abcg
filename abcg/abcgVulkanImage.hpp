/**
 * @file abcgVulkanImage.hpp
 * @brief Header file of abcg::VulkanImage
 *
 * Declaration of abcg::VulkanImage
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_VULKAN_IMAGE_HPP_
#define ABCG_VULKAN_IMAGE_HPP_

#include "abcgVulkanDevice.hpp"

#include <gsl/pointers>

namespace abcg {
struct VulkanImageCreateInfo;
class VulkanImage;
} // namespace abcg

/**
 * @brief Creation info structure for abcg::VulkanImage
 */
struct abcg::VulkanImageCreateInfo {
  vk::ImageCreateInfo info{};
  vk::MemoryPropertyFlags properties{};
  vk::ImageViewCreateInfo viewInfo{};
};

/**
 * @brief A class for representing a Vulkan image.
 *
 * This class provides helper functions for creating and managing vk::Image
 * objects.
 */
class abcg::VulkanImage {
public:
  void create(VulkanDevice const &device, std::string_view path,
              bool generateMipmaps = true);
  void create(VulkanDevice const &device,
              VulkanImageCreateInfo const &createInfo);
  void destroy();

  explicit operator vk::Image const &() const noexcept;

  [[nodiscard]] vk::DeviceMemory const &getDeviceMemory() const noexcept;
  [[nodiscard]] vk::ImageView const &getView() const noexcept;
  [[nodiscard]] vk::DescriptorImageInfo const &
  getDescriptorImageInfo() const noexcept;
  [[nodiscard]] uint32_t getMipLevels() const noexcept;

private:
  [[nodiscard]] std::pair<vk::Image, vk::DeviceMemory>
  createImage(VulkanDevice const &device, vk::ImageCreateInfo const &imageInfo,
              vk::MemoryPropertyFlags properties) const;
  void transitionImageLayout(VulkanDevice const &device,
                             vk::ImageLayout oldImageLayout,
                             vk::ImageLayout newImageLayout,
                             vk::ImageSubresourceRange subresourceRange = {
                                 .aspectMask = vk::ImageAspectFlagBits::eColor,
                                 .levelCount = 1,
                                 .layerCount = 1}) const;

  static void createMipmaps(VulkanDevice const &device, vk::Image image,
                            vk::Format imageFormat, uint32_t texWidth,
                            uint32_t texHeight, uint32_t mipLevels);

  vk::Image m_image;
  vk::DeviceMemory m_deviceMemory;
  vk::ImageView m_imageView;
  vk::Sampler m_sampler;
  vk::DescriptorImageInfo m_descriptorImageInfo;
  uint32_t m_mipLevels{1U};
  vk::Device m_device;
};

#endif