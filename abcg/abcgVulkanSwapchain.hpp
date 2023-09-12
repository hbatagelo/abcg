/**
 * @file abcgVulkanSwapchain.hpp
 * @brief Header file of abcg::VulkanSwapchain
 *
 * Declaration of abcg::VulkanSwapchain.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_VULKAN_SWAPCHAIN_HPP_
#define ABCG_VULKAN_SWAPCHAIN_HPP_

#include <functional>
#include <glm/fwd.hpp>

#include "abcgVulkanDevice.hpp"
#include "abcgVulkanImage.hpp"

namespace abcg {
class VulkanSwapchain;
struct VulkanFrame;
struct VulkanSettings;
class VulkanPipeline;
class VulkanWindow;
} // namespace abcg

/**
 * @brief Data needed by a rendering frame.
 *
 */
struct abcg::VulkanFrame {
  uint32_t index{};
  vk::CommandPool commandPool;
  vk::CommandBuffer commandBuffer;
  vk::CommandBuffer commandBufferUI;
  vk::Fence fence;
  VulkanImage colorImage;
  vk::Framebuffer framebufferMain;
};

/**
 * @brief A class for representing a Vulkan swapchain.
 *
 * This class creates and manages the list of image buffers and other resources
 * that are used for presentation.
 */
class abcg::VulkanSwapchain {
public:
  void create(VulkanDevice const &device, VulkanSettings const &settings,
              glm::ivec2 const &windowSize);
  void destroy();
  void render(std::function<void(VulkanFrame const &)> const &fun);
  void present();
  bool checkRebuild(VulkanSettings const &settings,
                    glm::ivec2 const &windowSize);

  explicit operator vk::SwapchainKHR const &() const noexcept;

  [[nodiscard]] VulkanDevice const &getDevice() const noexcept;
  [[nodiscard]] std::vector<VulkanFrame> const &getFrames() const noexcept;
  [[nodiscard]] VulkanFrame const &getCurrentFrame() const noexcept;
  [[nodiscard]] vk::RenderPass const &getMainRenderPass() const noexcept;
  [[nodiscard]] vk::RenderPass const &getUIRenderPass() const noexcept;
  [[nodiscard]] vk::Extent2D const &getExtent() const noexcept;
  [[nodiscard]] VulkanImage const &getDepthImage() const noexcept;

private:
  void createFrames();
  void destroyFrames();

  [[nodiscard]] vk::Format getDepthFormat(VulkanSettings const &settings);
  void createDepthResources(VulkanSettings const &settings);
  void destroyDepthResources();

  void createMSAAResources();
  void destroyMSAAResources();

  void createRenderPasses(VulkanSettings const &settings);
  void destroyRenderPasses();

  void createFramebuffers(VulkanSettings const &settings);

  vk::SwapchainKHR m_swapchainKHR;
  VulkanDevice m_device;

  vk::Format m_swapchainImageFormat;
  vk::Extent2D m_swapchainExtent;
  bool m_swapChainRebuild{};

  // Data for swapchain synchronization
  struct FrameSemaphores {
    vk::Semaphore presentComplete;
    vk::Semaphore renderComplete;
  };

  uint32_t m_currentFrame{};
  std::vector<VulkanFrame> m_frames;
  // Current set of swapchain wait semaphores we're using (needs to be distinct
  // from per frame data)
  uint32_t m_currentSemaphore{};
  std::vector<FrameSemaphores> m_frameSemaphores;

  VulkanImage m_depthImage;
  VulkanImage m_MSAAImage;

  // Render passes
  vk::RenderPass m_renderPassMain;
  vk::RenderPass m_renderPassUI;
};

#endif