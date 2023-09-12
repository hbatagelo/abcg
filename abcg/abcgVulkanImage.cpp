/**
 * @file abcgVulkanImage.cpp
 * @brief Definition of abcg::VulkanImage
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgVulkanImage.hpp"
#include "abcgVulkanBuffer.hpp"

#include <SDL_image.h>
#include <cppitertools/itertools.hpp>
#include <fmt/core.h>
#include <gsl/gsl>

#include "abcgException.hpp"

void abcg::VulkanImage::create(VulkanDevice const &device,
                               std::string_view path, bool generateMipmaps) {
  m_device = static_cast<vk::Device>(device);

  // Load the bitmap
  if (SDL_Surface *const surface{IMG_Load(path.data())}) {
    // Enforce RGBA
    SDL_Surface *formattedSurface{
        SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0)};
    SDL_FreeSurface(surface);

    auto const texWidth{gsl::narrow<uint32_t>(formattedSurface->w)};
    auto const texHeight{gsl::narrow<uint32_t>(formattedSurface->h)};
    vk::DeviceSize const imageSize{
        static_cast<vk::DeviceSize>(texWidth * texHeight * 4)};

    if (generateMipmaps) {
      m_mipLevels = gsl::narrow<uint32_t>(
                        std::floor(std::log2(std::max(texWidth, texHeight)))) +
                    1;
    }

    // Create staging buffer
    abcg::VulkanBuffer stagingBuffer{};
    stagingBuffer.create(
        device, {.size = imageSize,
                 .usage = vk::BufferUsageFlagBits::eTransferSrc,
                 .properties = vk::MemoryPropertyFlagBits::eHostVisible |
                               vk::MemoryPropertyFlagBits::eHostCoherent,
                 .data = formattedSurface->pixels});

    SDL_FreeSurface(formattedSurface);

    // TODO: Look for other formats if RGBA8 is not supported
    auto const imageFormat{vk::Format::eR8G8B8A8Srgb};

    // Create image buffer
    std::tie(m_image, m_deviceMemory) = createImage(
        device,
        {.imageType = vk::ImageType::e2D,
         .format = imageFormat,
         .extent = {.width = texWidth, .height = texHeight, .depth = 1},
         .mipLevels = m_mipLevels,
         .arrayLayers = 1,
         .samples = vk::SampleCountFlagBits::e1,
         .tiling = vk::ImageTiling::eOptimal,
         .usage = (m_mipLevels > 1 // Required for blit ops
                       ? vk::ImageUsageFlagBits::eTransferSrc
                       : vk::ImageUsageFlagBits::eTransferDst) |
                  vk::ImageUsageFlagBits::eTransferDst |
                  vk::ImageUsageFlagBits::eSampled,
         .initialLayout = vk::ImageLayout::eUndefined},
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    transitionImageLayout(device, vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eTransferDstOptimal,
                          {.aspectMask = vk::ImageAspectFlagBits::eColor,
                           .levelCount = m_mipLevels,
                           .layerCount = 1});

    vk::BufferImageCopy const region{
        .imageSubresource = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                             .layerCount = 1},
        .imageExtent = {texWidth, texHeight, 1}};

    // Execute the layout transition
    device.withCommandBuffer(
        [&stagingBuffer, this,
         &region](vk::CommandBuffer const &commandBuffer) {
          commandBuffer.copyBufferToImage(
              static_cast<vk::Buffer>(stagingBuffer), m_image,
              vk::ImageLayout::eTransferDstOptimal, region);
        },
        vk::QueueFlagBits::eTransfer);

    // Generate the mipmap levels
    if (m_mipLevels > 1) {
      // Transitioned to vk::ImageLayout::eShaderReadOnlyOptimal while
      // generating the mipmaps
      createMipmaps(device, m_image, imageFormat, texWidth, texHeight,
                    m_mipLevels);
    } else {
      transitionImageLayout(device, vk::ImageLayout::eTransferDstOptimal,
                            vk::ImageLayout::eReadOnlyOptimal,
                            {.aspectMask = vk::ImageAspectFlagBits::eColor,
                             .levelCount = m_mipLevels,
                             .layerCount = 1});
    }

    stagingBuffer.destroy();

    // Create image view
    m_imageView = m_device.createImageView(
        {.image = m_image,
         .viewType = vk::ImageViewType::e2D,
         .format = imageFormat,
         .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                              .levelCount = 1,
                              .layerCount = 1}});

    // Create sampler
    vk::SamplerCreateInfo samplerCreateInfo{
        .magFilter = vk::Filter::eLinear,
        .minFilter = vk::Filter::eLinear,
        .mipmapMode = vk::SamplerMipmapMode::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat,
        .addressModeV = vk::SamplerAddressMode::eRepeat,
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy =
            static_cast<vk::PhysicalDevice>(device.getPhysicalDevice())
                .getProperties()
                .limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = vk::CompareOp::eAlways,
        .minLod = 0.0f,
        .maxLod = 0.0f,
        .borderColor = vk::BorderColor::eIntOpaqueBlack,
        .unnormalizedCoordinates = VK_FALSE};

    if (m_mipLevels > 1) {
      samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
      samplerCreateInfo.maxLod = gsl::narrow<float>(m_mipLevels);
      // samplerCreateInfo.minLod = gsl::narrow<float>(m_mipLevels >> 1);
    }
    m_sampler = m_device.createSampler(samplerCreateInfo);

    // Create descriptor info
    m_descriptorImageInfo = {.sampler = m_sampler,
                             .imageView = m_imageView,
                             .imageLayout =
                                 vk::ImageLayout::eShaderReadOnlyOptimal};
  } else {
    throw abcg::RuntimeError(
        fmt::format("Failed to load texture file {}", path));
  }
}

void abcg::VulkanImage::create(VulkanDevice const &device,
                               VulkanImageCreateInfo const &createInfo) {
  m_device = static_cast<vk::Device>(device);

  // Create image only if createInfo.viewInfo.image is undefined
  if (!createInfo.viewInfo.image) {
    std::tie(m_image, m_deviceMemory) =
        createImage(device, createInfo.info, createInfo.properties);
  }

  // Create view if viewInfo.format is defined
  if (createInfo.viewInfo.format != vk::Format::eUndefined) {
    vk::ImageViewCreateInfo viewCreateInfo{createInfo.viewInfo};
    viewCreateInfo.image = m_image ? m_image : createInfo.viewInfo.image;
    m_imageView = m_device.createImageView(viewCreateInfo);
  }
}

void abcg::VulkanImage::destroy() {
  if (m_sampler) {
    m_device.destroySampler(m_sampler);
  }
  if (m_imageView) {
    m_device.destroyImageView(m_imageView);
  }
  if (m_image) {
    m_device.destroyImage(m_image);
  }
  if (m_deviceMemory) {
    m_device.freeMemory(m_deviceMemory);
  }
}

/**
 * @brief Conversion to vk::Image.
 */
abcg::VulkanImage::operator vk::Image const &() const noexcept {
  return m_image;
}

/**
 * @brief Returns the opaque handle to the device memory object associated
 * with this image.
 *
 * @return Device memory object.
 */
vk::DeviceMemory const &abcg::VulkanImage::getDeviceMemory() const noexcept {
  return m_deviceMemory;
}

/**
 * @brief Returns the opaque handle to the image view object associated with
 * this image.
 *
 * @return Image view object.
 */
vk::ImageView const &abcg::VulkanImage::getView() const noexcept {
  return m_imageView;
}

/**
 * @brief Returns the descriptor image information associated with this image.
 *
 * @return Descriptor image information.
 */
vk::DescriptorImageInfo const &
abcg::VulkanImage::getDescriptorImageInfo() const noexcept {
  return m_descriptorImageInfo;
}

/**
 * @brief Returns the number of mipmap levels generated for this image.
 *
 * If the image is created with `generateMipmaps = false`, the number of
 * mipmap levels is always 1. Otherwise, it is computed as \f$\lfloor
 * \log_2(\max(w, h)) \rfloor + 1\f$, where \f$w\f$ and \f$h\f$ are the
 * texture width and height.
 *
 * @return Number of mipmap levels.
 */
uint32_t abcg::VulkanImage::getMipLevels() const noexcept {
  return m_mipLevels;
}

std::pair<vk::Image, vk::DeviceMemory>
abcg::VulkanImage::createImage(VulkanDevice const &device,
                               vk::ImageCreateInfo const &imageInfo,
                               vk::MemoryPropertyFlags properties) const {
  // Create image object
  auto image{m_device.createImage(imageInfo)};

  // Get memory requirements
  auto const memoryRequirements{m_device.getImageMemoryRequirements(image)};

  // Allocate image memory
  auto const memoryType{device.getPhysicalDevice().findMemoryType(
      memoryRequirements.memoryTypeBits, properties)};
  if (!memoryType.has_value()) {
    throw abcg::RuntimeError("Failed to find suitable memory type");
  }
  auto imageMemory{
      m_device.allocateMemory({.allocationSize = memoryRequirements.size,
                               .memoryTypeIndex = memoryType.value()})};

  // Associate image memory to image
  m_device.bindImageMemory(image, imageMemory, 0);

  return {image, imageMemory};
}

void abcg::VulkanImage::transitionImageLayout(
    VulkanDevice const &device, vk::ImageLayout oldImageLayout,
    vk::ImageLayout newImageLayout,
    vk::ImageSubresourceRange subresourceRange) const {

  // Gets the corresponding access mask for a given image layout
  auto accessMask{[](vk::ImageLayout layout) {
    switch (layout) {
    case vk::ImageLayout::eColorAttachmentOptimal:
      return vk::AccessFlagBits::eColorAttachmentWrite;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
      return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
      return vk::AccessFlagBits::eShaderRead;
    case vk::ImageLayout::eTransferSrcOptimal:
      return vk::AccessFlagBits::eTransferRead;
    case vk::ImageLayout::eTransferDstOptimal:
      return vk::AccessFlagBits::eTransferWrite;
    case vk::ImageLayout::ePreinitialized:
      return vk::AccessFlagBits::eHostWrite;
    default:
      return vk::AccessFlagBits::eNone;
    }
  }};

  // Gets the corresponding pipeline stage for a given image layout
  auto stageMask{[](vk::ImageLayout layout) {
    switch (layout) {
    case vk::ImageLayout::eTransferDstOptimal:
    case vk::ImageLayout::eTransferSrcOptimal:
      return vk::PipelineStageFlagBits::eTransfer;
    case vk::ImageLayout::eColorAttachmentOptimal:
      return vk::PipelineStageFlagBits::eColorAttachmentOutput;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
      return vk::PipelineStageFlagBits::eEarlyFragmentTests;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
      return vk::PipelineStageFlagBits::eFragmentShader;
    case vk::ImageLayout::ePreinitialized:
      return vk::PipelineStageFlagBits::eHost;
    case vk::ImageLayout::eUndefined:
      return vk::PipelineStageFlagBits::eTopOfPipe;
    default:
      return vk::PipelineStageFlagBits::eBottomOfPipe;
    }
  }};

  // Create image barrier
  vk::ImageMemoryBarrier imageMemoryBarrier{
      .srcAccessMask = accessMask(oldImageLayout),
      .dstAccessMask = accessMask(newImageLayout),
      .oldLayout = oldImageLayout,
      .newLayout = newImageLayout,
      .image = m_image,
      .subresourceRange = subresourceRange};
  auto srcStageMask{stageMask(oldImageLayout)};
  auto destStageMask{stageMask(newImageLayout)};

  // Execute the layout transition
  device.withCommandBuffer(
      [&](const auto &commandBuffer) {
        commandBuffer.pipelineBarrier(srcStageMask, destStageMask,
                                      vk::DependencyFlags(), nullptr, nullptr,
                                      imageMemoryBarrier);
      },
      vk::QueueFlagBits::eTransfer);
}

void abcg::VulkanImage::createMipmaps(VulkanDevice const &device,
                                      vk::Image image, vk::Format imageFormat,
                                      uint32_t texWidth, uint32_t texHeight,
                                      uint32_t mipLevels) {
  // Check if image format supports linear blitting
  vk::FormatProperties const formatProperties{
      static_cast<vk::PhysicalDevice>(device.getPhysicalDevice())
          .getFormatProperties(imageFormat)};

  if (!(formatProperties.optimalTilingFeatures &
        vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
    // TODO: generate mip maps in software
    throw abcg::RuntimeError(
        "Texture image format does not support linear blitting");
  }

  device.withCommandBuffer(
      [&](vk::CommandBuffer const &commandBuffer) {
        vk::ImageMemoryBarrier barrier{
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                                 .levelCount = 1,
                                 .baseArrayLayer = 0,
                                 .layerCount = 1}};

        auto mipWidth{gsl::narrow<int32_t>(texWidth)};
        auto mipHeight{gsl::narrow<int32_t>(texHeight)};

        for (auto const mipLevel : iter::range(1U, mipLevels)) {
          barrier.subresourceRange.baseMipLevel = mipLevel - 1;
          barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
          barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
          barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
          barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

          commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                        vk::PipelineStageFlagBits::eTransfer,
                                        vk::DependencyFlagBits{}, {}, {},
                                        {{barrier}});

          vk::ImageBlit blit{};
          blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
          blit.srcOffsets[1] = vk::Offset3D{mipWidth, mipHeight, 1};
          blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
          blit.srcSubresource.mipLevel = mipLevel - 1;
          blit.srcSubresource.baseArrayLayer = 0;
          blit.srcSubresource.layerCount = 1;
          blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
          blit.dstOffsets[1] =
              vk::Offset3D{mipWidth > 1 ? mipWidth / 2 : 1,
                           mipHeight > 1 ? mipHeight / 2 : 1, 1};
          blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
          blit.dstSubresource.mipLevel = mipLevel;
          blit.dstSubresource.baseArrayLayer = 0;
          blit.dstSubresource.layerCount = 1;

          commandBuffer.blitImage(image, vk::ImageLayout::eTransferSrcOptimal,
                                  image, vk::ImageLayout::eTransferDstOptimal,
                                  {blit}, vk::Filter::eLinear);

          barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
          barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
          barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
          barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

          commandBuffer.pipelineBarrier(
              vk::PipelineStageFlagBits::eTransfer,
              vk::PipelineStageFlagBits::eFragmentShader,
              vk::DependencyFlagBits{}, {}, {}, {barrier});

          if (mipWidth > 1)
            mipWidth /= 2;
          if (mipHeight > 1)
            mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::DependencyFlagBits{}, {}, {}, {barrier});
      },
      vk::QueueFlagBits::eGraphics);
}
