/**
 * @file abcgVulkanWindow.cpp
 * @brief Definition of abcg::VulkanWindow members.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgVulkanWindow.hpp"

#include <SDL_vulkan.h>
#include <algorithm>
#include <gsl/gsl>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "abcgEmbeddedFonts.hpp"
#include "abcgException.hpp"
#include "abcgVulkanError.hpp"
#include "abcgVulkanInstance.hpp"
#include "abcgWindow.hpp"

namespace {
[[nodiscard]] std::vector<char const *>
getRequiredExtensions(gsl::not_null<SDL_Window *> window) {
  uint32_t extensionCount{};
  if (SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr) !=
      SDL_TRUE) {
    throw abcg::SDLError(
        "SDL_Vulkan_GetInstanceExtensions failed to get number of "
        "required extensions");
  }

  std::vector<char const *> extensions;
  if (extensionCount > 0) {
    extensions.resize(extensionCount);
    if (SDL_Vulkan_GetInstanceExtensions(window, &extensionCount,
                                         extensions.data()) != SDL_TRUE) {
      throw abcg::SDLError(
          "SDL_Vulkan_GetInstanceExtensions failed to get names of "
          "required extensions");
    }
  }

#if defined(ABCG_VULKAN_DEBUG_REPORT)
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  // fmt::print("Required extensions:\n");
  // for (const auto &extension : extensions) {
  //   fmt::print("\t{}\n", extension);
  // }
#endif

  return extensions;
}

void checkVkResultSingleArg(VkResult retCode) { abcg::checkVkResult(retCode); }
} // namespace

/**
 * @brief Returns the configuration settings of the Vulkan instance.
 *
 * @returns Reference to the abcg::VulkanSettings structure used for creating
 * the Vulkan instance.
 */
abcg::VulkanSettings const &
abcg::VulkanWindow::getVulkanSettings() const noexcept {
  return m_vulkanSettings;
}

/**
 * @brief Sets the configuration settings that will be used for creating the
 * Vulkan instance.
 *
 * This function will have no effect if called after the creation of the
 * Vulkan instance.
 */
void abcg::VulkanWindow::setVulkanSettings(
    VulkanSettings const &vulkanSettings) noexcept {
  if (abcg::Window::getSDLWindow() != nullptr)
    return;
  m_vulkanSettings = vulkanSettings;
}

/**
 * @brief Access to abcg::VulkanPhysicalDevice.
 *
 * @return Instance of vulkan physical device associated with this window.
 */
abcg::VulkanPhysicalDevice const &
abcg::VulkanWindow::getPhysicalDevice() const noexcept {
  return m_physicalDevice;
}
/**
 * @brief Access to abcg::VulkanDevice.
 *
 * @return Instance of vulkan device associated with this window.
 */
abcg::VulkanDevice const &abcg::VulkanWindow::getDevice() const noexcept {
  return m_device;
}
/**
 * @brief Access to abcg::VulkanSwapchain.
 *
 * @return Instance of swapchain associated with this window.
 */
abcg::VulkanSwapchain const &abcg::VulkanWindow::getSwapchain() const noexcept {
  return m_swapchain;
}

/**
 * @brief Custom event handler.
 *
 * This virtual function is called whenever there is a pending event polled by
 * SDL.
 *
 * Override it for custom behavior. By default, it does nothing.
 *
 * @param event Event structure containing information about the pending event.
 */
void abcg::VulkanWindow::onEvent([[maybe_unused]] SDL_Event const &event) {}

/**
 * @brief Custom handler for Vulkan initialization tasks to be performed before
 * rendering the scene.
 *
 * This virtual function is called once after the Vulkan instance is created.
 *
 * Override it for custom behavior. By default, it does nothing.
 */
void abcg::VulkanWindow::onCreate() {}

/**
 * @brief Custom handler for rendering the Vulkan scene.
 *
 * This virtual function is called for each frame of the rendering loop, just
 * after abcg::VulkanWindow::onPaintUI.
 *
 * This is not called when the window is minimized.
 *
 * @param frame Acquired in-flight frame.
 *
 * Override it for custom behavior. By default, it does nothing.
 */
void abcg::VulkanWindow::onPaint([[maybe_unused]] VulkanFrame const &frame) {}

/**
 * @brief Custom handler for rendering Dear ImGUI controls.
 *
 * This virtual function is called for each frame in the rendering loop, just
 * before abcg::VulkanWindow::onPaint.
 *
 * This is not called when the window is minimized.
 *
 * Override it for custom behavior. By default, it shows a FPS counter if
 * abcg::WindowSettings::showFPS is set to `true`, and a toggle fullscreen
 * button if abcg::WindowSettings::showFullscreenButton is set to `true`.
 */
void abcg::VulkanWindow::onPaintUI() {
  // FPS counter
  if (abcg::Window::getWindowSettings().showFPS) {
    auto fps{ImGui::GetIO().Framerate};

    static auto offset{0UL};
    static auto refreshTime{ImGui::GetTime()};
    static std::array<float, 150> frames{};

    while (refreshTime < ImGui::GetTime()) {
      auto const refreshFrequency{60.0};
      frames.at(offset) = fps;
      offset = (offset + 1) % frames.size();
      refreshTime += (1.0 / refreshFrequency);
    }

    ImGui::SetNextWindowPos(ImVec2(5, 5));
    ImGui::Begin("FPS", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoFocusOnAppearing);
    auto const label{fmt::format("avg {:.1f} FPS", fps)};
    ImGui::PlotLines("", frames.data(), gsl::narrow<int>(frames.size()),
                     gsl::narrow<int>(offset), label.c_str(), 0.0f,
                     *std::ranges::max_element(frames) * 2,
                     ImVec2(gsl::narrow<float>(frames.size()), 50));
    ImGui::End();
  }

  // Fullscreen button
  if (abcg::Window::getWindowSettings().showFullscreenButton) {
    auto const windowSize{getWindowSize()};
    auto const widgetSize{ImVec2(150, 30)};
    auto const windowBorder{ImVec2(16, 16)};

    ImGui::SetNextWindowSize(
        ImVec2(widgetSize.x + windowBorder.x, widgetSize.y + windowBorder.y));
    ImGui::SetNextWindowPos(ImVec2(5, gsl::narrow<float>(windowSize.y) -
                                          (widgetSize.y + windowBorder.y) - 5));

    auto const windowFlags{ImGuiWindowFlags_NoDecoration |
                           ImGuiWindowFlags_NoBringToFrontOnFocus |
                           ImGuiWindowFlags_NoFocusOnAppearing};
    ImGui::Begin("Fullscreen", nullptr, windowFlags);

    if (ImGui::Button("Toggle fullscreen", widgetSize)) {
      abcg::Window::toggleFullscreen();
    }

    ImGui::End();
  }
}

/**
 * @brief Custom handler for window resizing events.
 *
 * This virtual function is called whenever the window is resized.
 *
 * Override it for custom behavior. By default, it does nothing.
 */
void abcg::VulkanWindow::onResize() {}

/**
 * @brief Custom handler called for each frame before painting.
 *
 * This virtual function is called just before abcg::VulkanWindow::onPaint, even
 * if the window is minimized.
 *
 * Override it for custom behavior. By default, it does nothing.
 */
void abcg::VulkanWindow::onUpdate() {}

/**
 * @brief Custom handler for cleaning up Vulkan resources.
 *
 * This is a hook function called when the application is exiting.
 *
 * Override it for custom behavior. By default, it does nothing.
 */
void abcg::VulkanWindow::onDestroy() {}

void abcg::VulkanWindow::handleEvent(SDL_Event const &event) {
  if (event.window.windowID != abcg::Window::getSDLWindowID())
    return;

  if (event.type == SDL_WINDOWEVENT) {
    switch (event.window.event) {
    case SDL_WINDOWEVENT_HIDDEN:
      m_hidden = true;
      break;
    case SDL_WINDOWEVENT_SHOWN:
    case SDL_WINDOWEVENT_EXPOSED:
      m_hidden = false;
      break;
    case SDL_WINDOWEVENT_MINIMIZED:
      m_minimized = true;
      break;
    case SDL_WINDOWEVENT_RESTORED:
      m_minimized = false;
      break;
    default:
      break;
    }
  }

  onEvent(event);
}

void abcg::VulkanWindow::create() {
  // Create window fol Vulkan graphics
  if (!createSDLWindow(SDL_WINDOW_VULKAN)) {
    throw abcg::SDLError("SDL_CreateWindow failed");
  }

  // Create Vulkan instance
  auto const applicationName{abcg::Window::getWindowSettings().title};
  auto const requiredExtensions{getRequiredExtensions(Window::getSDLWindow())};
  m_instance.create(m_layers, requiredExtensions, applicationName);

  // Create window surface
  if (VkSurfaceKHR surface{};
      SDL_Vulkan_CreateSurface(abcg::Window::getSDLWindow(),
                               static_cast<vk::Instance>(m_instance),
                               &surface) == SDL_TRUE) {
    m_surface = vk::SurfaceKHR(surface);
  } else {
    throw abcg::SDLError("Failed to create window surface");
  }

  // Make sure the desired sample count is a power of two between 1 and 64
  uint32_t exponent{};
  auto samples{std::clamp(gsl::narrow<uint32_t>(m_vulkanSettings.samples),
                          gsl::narrow<uint32_t>(vk::SampleCountFlagBits::e1),
                          gsl::narrow<uint32_t>(vk::SampleCountFlagBits::e64))};
  while ((samples >>= 1) > 0) {
    ++exponent;
  }
  auto const sampleCount{gsl::narrow<vk::SampleCountFlagBits>(1 << exponent)};

  // Select physical device
  m_physicalDevice.create(m_instance, m_surface, m_deviceExtensions,
                          sampleCount);

  // Create logical device
  m_device.create(m_physicalDevice, m_deviceExtensions);

  // Create swapchain
  m_swapchain.create(m_device, m_vulkanSettings, getWindowSize());

  // Create descriptor pool
  std::vector<vk::DescriptorPoolSize> const poolSizes{
      {{vk::DescriptorType::eSampler, 100},
       {vk::DescriptorType::eCombinedImageSampler, 100},
       {vk::DescriptorType::eSampledImage, 100},
       {vk::DescriptorType::eStorageImage, 100},
       {vk::DescriptorType::eUniformTexelBuffer, 100},
       {vk::DescriptorType::eStorageTexelBuffer, 100},
       {vk::DescriptorType::eUniformBuffer, 100},
       {vk::DescriptorType::eStorageBuffer, 100},
       {vk::DescriptorType::eUniformBufferDynamic, 100},
       {vk::DescriptorType::eStorageBufferDynamic, 100},
       {vk::DescriptorType::eInputAttachment, 100}}};

  m_UIdescriptorPool = static_cast<vk::Device>(m_device).createDescriptorPool(
      {.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
       .maxSets = gsl::narrow<uint32_t>(100 * poolSizes.size()),
       .poolSizeCount = gsl::narrow<uint32_t>(poolSizes.size()),
       .pPoolSizes = poolSizes.data()});

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &guiIO{ImGui::GetIO()};
  // Enable keyboard controls
  guiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // Enable gamepad controls
  guiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  // Disable ini files
  guiIO.IniFilename = nullptr;

  // Setup platform/renderer bindings
  ImGui_ImplSDL2_InitForVulkan(getSDLWindow());

  ImGui_ImplVulkan_LoadFunctions([](const char *function_name, void *) {
    return vkGetInstanceProcAddr(volkGetLoadedInstance(), function_name);
  });

  ImGui_ImplVulkan_InitInfo imGuiInitInfo{
      .Instance = static_cast<vk::Instance>(m_instance),
      .PhysicalDevice = static_cast<vk::PhysicalDevice>(m_physicalDevice),
      .Device = static_cast<vk::Device>(m_device),
      .QueueFamily = m_physicalDevice.getQueuesFamilies().graphics.value_or(0),
      .Queue = m_device.getQueues().graphics,
      .PipelineCache = VK_NULL_HANDLE,
      .DescriptorPool = m_UIdescriptorPool,
      .Subpass = 0,
      .MinImageCount = 2,
      .ImageCount = gsl::narrow<uint32_t>(m_swapchain.getFrames().size()),
      .MSAASamples =
          static_cast<VkSampleCountFlagBits>(m_physicalDevice.getSampleCount()),
      .Allocator = nullptr,
      .CheckVkResultFn = checkVkResultSingleArg};
  ImGui_ImplVulkan_Init(&imGuiInitInfo, m_swapchain.getUIRenderPass());

  // Load fonts
  guiIO.Fonts->Clear();
  ImFontConfig fontConfig{};
  fontConfig.FontDataOwnedByAtlas = false;
  if (std::array ttf{INCONSOLATA_MEDIUM_TTF};
      guiIO.Fonts->AddFontFromMemoryTTF(ttf.data(),
                                        gsl::narrow<int>(ttf.size()), 16.0f,
                                        &fontConfig) == nullptr) {
    throw abcg::RuntimeError("Failed to load font file");
  }

  // Upload fonts
  {
    auto const &commandBuffer{m_swapchain.getCurrentFrame().commandBuffer};

    commandBuffer.begin(
        {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

    commandBuffer.end();

    m_device.getQueues().graphics.submit(vk::SubmitInfo{
        .commandBufferCount = 1, .pCommandBuffers = &commandBuffer});

    static_cast<vk::Device>(m_device).waitIdle();

    ImGui_ImplVulkan_DestroyFontUploadObjects();
  }

  onCreate();

  onResize();
}

void abcg::VulkanWindow::paint() {
  onUpdate();

  if (m_hidden || m_minimized)
    return;

  if (m_swapchain.checkRebuild(m_vulkanSettings, getWindowSize())) {
    onResize();
  }

  // ImGUI requires at least 2 images in the swapchain
  ImGui_ImplVulkan_SetMinImageCount(2);

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  onPaintUI();

  ImGui::Render();

  m_swapchain.render([this](auto const &frame) { onPaint(frame); });
  m_swapchain.present();
}

void abcg::VulkanWindow::destroy() {
  static_cast<vk::Device>(m_device).waitIdle();

  onDestroy();

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  static_cast<vk::Device>(m_device).destroyDescriptorPool(m_UIdescriptorPool);
  m_swapchain.destroy();
  m_device.destroy();
  m_physicalDevice.destroy();
  static_cast<vk::Instance>(m_instance).destroySurfaceKHR(m_surface);
  m_instance.destroy();
}

glm::ivec2 abcg::VulkanWindow::getWindowSize() const {
  glm::ivec2 size{};

  if (auto *window{abcg::Window::getSDLWindow()}) {
    SDL_Vulkan_GetDrawableSize(window, &size.x, &size.y);
  }
  return size;
}