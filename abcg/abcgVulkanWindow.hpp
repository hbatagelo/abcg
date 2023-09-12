/**
 * @file abcgVulkanWindow.hpp
 * @brief Header file of abcg::VulkanWindow.
 *
 * Declaration of abcg::VulkanWindow.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_VULKAN_WINDOW_HPP_
#define ABCG_VULKAN_WINDOW_HPP_

#include "abcgVulkanDevice.hpp"
#include "abcgVulkanInstance.hpp"
#include "abcgVulkanPhysicalDevice.hpp"
#include "abcgVulkanSwapchain.hpp"
#include "abcgWindow.hpp"

namespace abcg {
class VulkanWindow;
struct VulkanSettings;
} // namespace abcg

/**
 * @brief Configuration settings for creating a Vulkan context.
 *
 * This structure contains the configuration settings to be used when creating
 * the Vulkan context. These must be set before calling
 * `abcg::Application::run`.
 *
 * @sa abcg::VulkanWindow::getVulkanSettings.
 * @sa abcg::VulkanWindow::setVulkanSettings.
 */
struct abcg::VulkanSettings {
  /** @brief Minimum number of bits in the depth buffer. */
  int depthBufferSize{24};

  /** @brief Minimum number of bits in the stencil buffer. */
  int stencilBufferSize{0};

  /** @brief Number of desired samples to be used for multisample anti-aliasing.
   *
   * The actual number of samples used by the physical device can be smaller.
   * MSAA will be disabled if the number of samples is zero.
   */
  int samples{0};

  /** @brief Whether to synchronize the presentation with the vertical retrace.
   *
   * If `true`, the internal present mode is set to the mailbox mode. If the
   * mailbox mode is not supported, FIFO mode is used. If `false`, the internal
   * present mode is set to the immediate mode. If the immediate mode is not
   * supported, the behavior falls back to mailbox mode or FIFO mode, whichever
   * comes first.
   */
  bool vSync{false};
};

/**
 * @brief Base class for a window that displays graphics using a Vulkan
 * context.
 *
 * Derive from this class to create a custom Vulkan window object to be passed
 * to abcg::Application::run.
 *
 * @sa abcg::VulkanWindow::onEvent for handling SDL events.
 * @sa abcg::VulkanWindow::onCreate for initializing Vulkan resources.
 * @sa abcg::VulkanWindow::onPaint for scene rendering.
 * @sa abcg::VulkanWindow::onPaintUI for UI rendering.
 * @sa abcg::VulkanWindow::onResize for handling swapchain rebuild events.
 * @sa abcg::VulkanWindow::onUpdate for commands to be called every frame.
 * @sa abcg::VulkanWindow::onDestroy for cleaning up Vulkan resources.
 *
 * @remark Objects of this type cannot be copied or copy-constructed.
 */
class abcg::VulkanWindow : public Window {
public:
  [[nodiscard]] VulkanSettings const &getVulkanSettings() const noexcept;
  void setVulkanSettings(VulkanSettings const &vulkanSettings) noexcept;
  // void saveScreenshotPNG(std::string_view filename) const;
  [[nodiscard]] VulkanPhysicalDevice const &getPhysicalDevice() const noexcept;
  [[nodiscard]] VulkanDevice const &getDevice() const noexcept;
  [[nodiscard]] VulkanSwapchain const &getSwapchain() const noexcept;

protected:
  virtual void onEvent(SDL_Event const &event);
  virtual void onCreate();
  virtual void onPaint(VulkanFrame const &frame);
  virtual void onPaintUI();
  virtual void onResize();
  virtual void onUpdate();
  virtual void onDestroy();

private:
  void handleEvent(SDL_Event const &event) final;
  void create() final;
  void paint() final;
  void destroy() final;
  [[nodiscard]] glm::ivec2 getWindowSize() const final;

  VulkanSettings m_vulkanSettings;
  std::vector<char const *> m_deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  std::vector<char const *> m_layers {
#if defined(ABCG_VULKAN_DEBUG_REPORT)
    "VK_LAYER_KHRONOS_validation"
#endif
  };

  VulkanInstance m_instance;
  VulkanPhysicalDevice m_physicalDevice;
  VulkanDevice m_device;
  VulkanSwapchain m_swapchain;
  vk::SurfaceKHR m_surface;
  vk::DescriptorPool m_UIdescriptorPool;
  bool m_hidden{};
  bool m_minimized{};
};

#endif
