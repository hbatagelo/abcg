/**
 * @file abcgVulkanSwapchain.cpp
 * @brief Definition of abcg::VulkanSwapchain
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgVulkanSwapchain.hpp"

#include <functional>
#include <gsl/gsl>
#include <imgui_impl_vulkan.h>

#include "abcgException.hpp"
#include "abcgVulkanDevice.hpp"
#include "abcgVulkanPhysicalDevice.hpp"
#include "abcgVulkanWindow.hpp"

namespace {
struct SurfaceSupport {
  vk::SurfaceKHR surfaceKHR{};
  vk::SurfaceCapabilitiesKHR capabilities{};
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;
};

[[nodiscard]] vk::SurfaceFormatKHR
chooseSwapSurfaceFormat(std::vector<vk::Format> const &requestFormats,
                        vk::ColorSpaceKHR requestColorSpace,
                        SurfaceSupport const &surfaceSupport) {
  // Request several formats, the first found will be used
  for (auto const &requestFormat : requestFormats) {
    for (auto const &supportedFormat : surfaceSupport.formats) {
      if (supportedFormat.format == requestFormat &&
          supportedFormat.colorSpace == requestColorSpace) {
        return supportedFormat;
      }
    }
  }

  // No requested image formats could be found. Use the first supported
  return surfaceSupport.formats.front();
}

[[nodiscard]] vk::PresentModeKHR
chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &requestModes,
                      SurfaceSupport const &surfaceSupport) {
  // #if defined(ABCG_VULKAN_DEBUG_REPORT)
  //   fmt::print("Supported presentation modes:\n");
  //   for (auto const &mode : surfaceSupport.presentModes) {
  //     fmt::print("\t{}\n", vk::to_string(mode));
  //   }
  // #endif

  for (auto const &requestMode : requestModes) {
    for (auto const &supportedMode : surfaceSupport.presentModes) {
      if (supportedMode == requestMode)
        return requestMode;
    }
  }

  return vk::PresentModeKHR::eFifo; // Always supported
}

[[nodiscard]] uint32_t
getMinImageCountFromPresentMode(vk::PresentModeKHR presentMode) {
  switch (presentMode) {
  case vk::PresentModeKHR::eMailbox:
    return 3;
  case vk::PresentModeKHR::eFifo:
  case vk::PresentModeKHR::eFifoRelaxed:
    return 2;
  case vk::PresentModeKHR::eImmediate:
  case vk::PresentModeKHR::eSharedDemandRefresh:
  case vk::PresentModeKHR::eSharedContinuousRefresh:
  default:
    return 1;
  }
}

[[nodiscard]] vk::Extent2D
chooseSwapExtent(vk::SurfaceCapabilitiesKHR capabilities,
                 glm::ivec2 windowSize) {
  // If the surface size is defined, the swap chain size must match
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }

  // If the surface size is undefined, the size is set to the size of the images
  // requested
  return vk::Extent2D{.width = std::clamp(gsl::narrow<uint32_t>(windowSize.x),
                                          capabilities.minImageExtent.width,
                                          capabilities.maxImageExtent.width),
                      .height = std::clamp(gsl::narrow<uint32_t>(windowSize.y),
                                           capabilities.minImageExtent.height,
                                           capabilities.maxImageExtent.height)};
}
} // namespace

void abcg::VulkanSwapchain::create(VulkanDevice const &device,
                                   VulkanSettings const &settings,
                                   glm::ivec2 const &windowSize) {
  m_device = device;

  m_swapChainRebuild = true;

  checkRebuild(settings, windowSize);
}

void abcg::VulkanSwapchain::destroy() {
  auto const &device{static_cast<vk::Device>(m_device)};

  if (!device) {
    return;
  }

  destroyMSAAResources();
  destroyDepthResources();
  destroyFrames();
  destroyRenderPasses();

  device.destroySwapchainKHR(m_swapchainKHR);
}

void abcg::VulkanSwapchain::render(
    std::function<void(VulkanFrame const &)> const &fun) {
  auto const &device{static_cast<vk::Device>(m_device)};

  // Get current set of semaphores
  auto [presentCompleteSemaphore,
        renderCompleteSemaphore]{m_frameSemaphores.at(m_currentSemaphore)};

  // Acquire an image from the swapchain
  vk::Result result{};
  try {
    result = device.acquireNextImageKHR(
        m_swapchainKHR, std::numeric_limits<uint64_t>::max(),
        presentCompleteSemaphore, vk::Fence{}, &m_currentFrame);
  } catch (vk::OutOfDateKHRError const &) {
    result = vk::Result::eErrorOutOfDateKHR;
  }
  if (result == vk::Result::eErrorOutOfDateKHR ||
      result == vk::Result::eSuboptimalKHR) {
    m_swapChainRebuild = true;
    return;
  }

  auto const &frame{m_frames.at(m_currentFrame)};

  // Wait until command buffer for acquired image has finished executing
  while (vk::Result::eTimeout ==
         device.waitForFences(frame.fence, VK_TRUE,
                              std::numeric_limits<uint64_t>::max()))
    ;
  device.resetFences(frame.fence);
  device.resetCommandPool(frame.commandPool);

  // Main pass
  fun(frame);

  // UI render pass
  frame.commandBufferUI.begin(
      {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

  std::array<vk::ClearValue, 2> const clearValues{};

  frame.commandBufferUI.beginRenderPass(
      {.renderPass = m_renderPassUI,
       .framebuffer = frame.framebufferMain,
       .renderArea = {.offset{}, .extent{m_swapchainExtent}},
       .clearValueCount = clearValues.size(),
       .pClearValues = clearValues.data()},
      vk::SubpassContents::eInline);

  // Record Dear ImGUI primitives into command buffer
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frame.commandBufferUI);

  frame.commandBufferUI.endRenderPass();

  frame.commandBufferUI.end();

  std::array waitSemaphores{presentCompleteSemaphore};
  std::array waitStages{vk::PipelineStageFlags{
      vk::PipelineStageFlagBits::eColorAttachmentOutput}};
  std::array commandBuffers{frame.commandBuffer, frame.commandBufferUI};
  std::array signalSemaphores{renderCompleteSemaphore};

  // Submit command buffer
  m_device.getQueues().graphics.submit(
      {{.waitSemaphoreCount = gsl::narrow<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphores = waitSemaphores.data(),
        .pWaitDstStageMask = waitStages.data(),
        .commandBufferCount = commandBuffers.size(),
        .pCommandBuffers = commandBuffers.data(),
        .signalSemaphoreCount = gsl::narrow<uint32_t>(signalSemaphores.size()),
        .pSignalSemaphores = signalSemaphores.data()}},
      frame.fence);
}

void abcg::VulkanSwapchain::present() {
  if (m_swapChainRebuild)
    return;

  // Set semaphores to wait
  auto &frameSemaphore{m_frameSemaphores.at(m_currentSemaphore)};
  std::array waitSemaphores{frameSemaphore.renderComplete};

  // Set swapchains
  std::array swapchains{m_swapchainKHR};

  vk::Result result{};
  try {
    result = m_device.getQueues().present.presentKHR({
        .waitSemaphoreCount = gsl::narrow<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphores = waitSemaphores.data(),
        .swapchainCount = gsl::narrow<uint32_t>(swapchains.size()),
        .pSwapchains = swapchains.data(),
        .pImageIndices = &m_currentFrame // Index of in-flight frame
    });
  } catch (vk::OutOfDateKHRError const &) {
    result = vk::Result::eErrorOutOfDateKHR;
  }
  if (result == vk::Result::eErrorOutOfDateKHR ||
      result == vk::Result::eSuboptimalKHR) {
    m_swapChainRebuild = true;
    return;
  }

  // Use the next set of semaphores
  m_currentSemaphore =
      (m_currentSemaphore + 1) % gsl::narrow<uint32_t>(m_frames.size());
}

bool abcg::VulkanSwapchain::checkRebuild(VulkanSettings const &settings,
                                         glm::ivec2 const &windowSize) {
  if (!m_swapChainRebuild)
    return false;

  auto const &device{static_cast<vk::Device>(m_device)};

  auto oldSwapchain{m_swapchainKHR};
  m_swapchainKHR = vk::SwapchainKHR{};
  device.waitIdle();

  // Destroy old swapchain and in-flight frames data, if any
  destroy();

  auto const &physicalDevice{
      static_cast<vk::PhysicalDevice>(m_device.getPhysicalDevice())};
  auto const &surface{m_device.getPhysicalDevice().getSurfaceKHR()};

  SurfaceSupport const surfaceCaps{
      .capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface),
      .formats = physicalDevice.getSurfaceFormatsKHR(surface),
      .presentModes = physicalDevice.getSurfacePresentModesKHR(surface)};

  // Choose surface format
  std::vector const requestSurfaceFormats{
      // vk::Format::eB8G8R8Srgb,
      vk::Format::eB8G8R8A8Unorm, vk::Format::eR8G8B8A8Unorm,
      vk::Format::eB8G8R8Unorm, vk::Format::eR8G8B8Unorm};

  auto const requestSurfaceColorSpace{
      vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear};

  auto const surfaceFormat{chooseSwapSurfaceFormat(
      requestSurfaceFormats, requestSurfaceColorSpace, surfaceCaps)};

  m_swapchainImageFormat = surfaceFormat.format;

  // Choose present mode
  std::vector presentModes{vk::PresentModeKHR::eMailbox,
                           vk::PresentModeKHR::eFifo};
  if (!settings.vSync) {
    presentModes.insert(presentModes.begin(), vk::PresentModeKHR::eImmediate);
  }

  auto const presentMode{chooseSwapPresentMode(presentModes, surfaceCaps)};
  // #if defined(ABCG_VULKAN_DEBUG_REPORT)
  //   fmt::print("[vulkan] Selected mode: {}\n", vk::to_string(presentMode));
  // #endif

  // Request different count of images depending on the selected present mode
  auto minImageCount{std::max(surfaceCaps.capabilities.minImageCount,
                              getMinImageCountFromPresentMode(presentMode))};
  if (surfaceCaps.capabilities.maxImageCount > 0) {
    minImageCount =
        std::min(minImageCount, surfaceCaps.capabilities.maxImageCount);
  }

  // Choose extent
  m_swapchainExtent = chooseSwapExtent(surfaceCaps.capabilities, windowSize);
  if (m_swapchainExtent.width == 0 && m_swapchainExtent.height == 0)
    return false;

  auto const &supportedComposite{
      surfaceCaps.capabilities.supportedCompositeAlpha};
  auto const compositeAlpha{
      (supportedComposite & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
          ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
      : (supportedComposite & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
          ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
      : (supportedComposite & vk::CompositeAlphaFlagBitsKHR::eInherit)
          ? vk::CompositeAlphaFlagBitsKHR::eInherit
          : vk::CompositeAlphaFlagBitsKHR::eOpaque};

  // Create swapchain
  vk::SwapchainCreateInfoKHR createInfo{
      .surface = surface,
      .minImageCount = minImageCount,
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = m_swapchainExtent,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
      .preTransform = surfaceCaps.capabilities.currentTransform,
      .compositeAlpha = compositeAlpha,
      .presentMode = presentMode,
      .clipped = VK_TRUE,
      .oldSwapchain = oldSwapchain};

  auto const &queuesFamilies{m_device.getPhysicalDevice().getQueuesFamilies()};
  if (!queuesFamilies.graphics.has_value()) {
    throw abcg::RuntimeError("Graphics queue family not found");
  }

  if (!queuesFamilies.present.has_value()) {
    throw abcg::RuntimeError("Present queue family not found");
  }

  if (queuesFamilies.graphics.value() != queuesFamilies.present.value()) {
    // If the graphics and present queues are from different queue families, we
    // either have to explicitly transfer ownership of images between the
    // queues, or we have to create the swapchain with imageSharingMode as
    // vk::SharingMode::eConcurrent
    createInfo.imageSharingMode = vk::SharingMode::eConcurrent;

    std::array const queueFamilyIndices{queuesFamilies.graphics.value(),
                                        queuesFamilies.present.value()};
    createInfo.queueFamilyIndexCount =
        gsl::narrow<uint32_t>(queueFamilyIndices.size());
    createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
  } else {
    createInfo.imageSharingMode = vk::SharingMode::eExclusive;
  }

  m_swapchainKHR = device.createSwapchainKHR(createInfo);

  device.destroySwapchainKHR(oldSwapchain);

  createRenderPasses(settings);

  createFrames();

  if (settings.depthBufferSize > 0 || settings.stencilBufferSize > 0) {
    createDepthResources(settings);
  }

  if (m_device.getPhysicalDevice().getSampleCount() >
      vk::SampleCountFlagBits::e1) {
    createMSAAResources();
  }

  createFramebuffers(settings);

  m_swapChainRebuild = false;

  return true;
}

/**
 * @brief Conversion to vk::SwapchainKHR.
 */
abcg::VulkanSwapchain::operator vk::SwapchainKHR const &() const noexcept {
  return m_swapchainKHR;
}

/**
 * @brief Access to abcg::VulkanDevice.
 *
 * @return Instance of vulkan device associated with this swapchain.
 */
abcg::VulkanDevice const &abcg::VulkanSwapchain::getDevice() const noexcept {
  return m_device;
}

/**
 * @brief Returns the in-flight frames.
 *
 * @return Container of in-flight frames.
 */
std::vector<abcg::VulkanFrame> const &
abcg::VulkanSwapchain::getFrames() const noexcept {
  return m_frames;
}

/**
 * @brief Returns the current in-flight frame.
 *
 * @return Current frame being rendered to.
 */
abcg::VulkanFrame const &
abcg::VulkanSwapchain::getCurrentFrame() const noexcept {
  return m_frames[m_currentFrame];
}

/**
 * @brief Returns the main render pass.
 *
 * @return Instance of the main render pass.
 */
vk::RenderPass const &
abcg::VulkanSwapchain::getMainRenderPass() const noexcept {
  return m_renderPassMain;
}

/**
 * @brief Returns the UI render pass.
 *
 * @return Instance of the UI render pass.
 */
vk::RenderPass const &abcg::VulkanSwapchain::getUIRenderPass() const noexcept {
  return m_renderPassUI;
}

/**
 * @brief Returns the swapchain extent.
 *
 * @return Extent of the swapchain.
 */
vk::Extent2D const &abcg::VulkanSwapchain::getExtent() const noexcept {
  return m_swapchainExtent;
}

/**
 * @brief Returns the depth image object.
 *
 * @return Depth image object.
 */
abcg::VulkanImage const &abcg::VulkanSwapchain::getDepthImage() const noexcept {
  return m_depthImage;
}

void abcg::VulkanSwapchain::createFrames() {
  auto const swapchainImages{
      static_cast<vk::Device>(m_device).getSwapchainImagesKHR(m_swapchainKHR)};

  // Create image views
  m_currentFrame = 0;
  m_frames.resize(swapchainImages.size());
  m_currentSemaphore = 0;
  m_frameSemaphores.resize(swapchainImages.size());

  for (auto &&[frame, image, index] :
       iter::zip(m_frames, swapchainImages, iter::range(m_frames.size()))) {
    frame.index = gsl::narrow<uint32_t>(index);
    frame.colorImage.create(
        m_device,
        {.viewInfo = {
             .image = image,
             .viewType = vk::ImageViewType::e2D,
             .format = m_swapchainImageFormat,
             .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                                  .levelCount = 1,
                                  .layerCount = 1}}});
  }
}

void abcg::VulkanSwapchain::destroyFrames() {
  auto const &device{static_cast<vk::Device>(m_device)};

  for (auto &frame : m_frames) {
    device.destroyCommandPool(frame.commandPool);
    device.destroyFence(frame.fence);
    frame.colorImage.destroy();
    device.destroyFramebuffer(frame.framebufferMain);
  }

  for (auto &frameSemaphore : m_frameSemaphores) {
    device.destroySemaphore(frameSemaphore.presentComplete);
    device.destroySemaphore(frameSemaphore.renderComplete);
  }

  m_frames.clear();
  m_frameSemaphores.clear();
}

// TODO:
// take into account m_vulkanSettings.depthBufferSize and
// m_vulkanSettings.stencilBufferSize
vk::Format
abcg::VulkanSwapchain::getDepthFormat(VulkanSettings const &settings) {
  std::vector<vk::Format> candidateFormats;

  if (settings.depthBufferSize <= 0 && settings.stencilBufferSize <= 0) {
    // Neither depth nor stencil requested
    return vk::Format::eUndefined;
  }

  if (settings.depthBufferSize > 0 && settings.stencilBufferSize <= 0) {
    // Only depth requested
    std::array const formats{vk::Format::eD32Sfloat,
                             vk::Format::eD32SfloatS8Uint,
                             vk::Format::eD24UnormS8Uint, vk::Format::eD16Unorm,
                             vk::Format::eD16UnormS8Uint};
    std::ranges::copy(formats, std::back_inserter(candidateFormats));
  }

  if (settings.depthBufferSize <= 0 && settings.stencilBufferSize > 0) {
    // Only stencil requested
    std::array const formats{vk::Format::eS8Uint, vk::Format::eD16UnormS8Uint,
                             vk::Format::eD24UnormS8Uint,
                             vk::Format::eD32SfloatS8Uint};
    std::ranges::copy(formats, std::back_inserter(candidateFormats));
  }

  if (settings.depthBufferSize > 0 && settings.stencilBufferSize > 0) {
    // Both depth and stencil requested
    std::array const formats{vk::Format::eD32SfloatS8Uint,
                             vk::Format::eD24UnormS8Uint,
                             vk::Format::eD16UnormS8Uint};
    std::ranges::copy(formats, std::back_inserter(candidateFormats));
  }

  auto result{m_device.getPhysicalDevice().getFirstSupportedFormat(
      candidateFormats, vk::ImageTiling::eOptimal,
      vk::FormatFeatureFlagBits::eDepthStencilAttachment)};
  if (!result.has_value()) {
    throw abcg::RuntimeError("Failed to find depth format");
  }

  auto isResult{[&result](auto format) { return format == result.value(); }};

  // Check if required stencil size is supported
  if (settings.stencilBufferSize > 8 ||
      (settings.stencilBufferSize > 0 &&
       !std::ranges::any_of(std::array{vk::Format::eS8Uint,
                                       vk::Format::eD16UnormS8Uint,
                                       vk::Format::eD24UnormS8Uint,
                                       vk::Format::eD32SfloatS8Uint},
                            isResult))) {
    throw abcg::RuntimeError("Failed to find stencil format");
  }

  // Check if required depth size is supported
  if (settings.depthBufferSize > 32 ||
      (settings.depthBufferSize > 24 &&
       std::ranges::none_of(
           std::array{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint},
           isResult)) ||
      (settings.depthBufferSize > 16 &&
       std::ranges::none_of(std::array{vk::Format::eD32Sfloat,
                                       vk::Format::eD32SfloatS8Uint,
                                       vk::Format::eD24UnormS8Uint},
                            isResult)) ||
      (settings.depthBufferSize > 0 &&
       std::ranges::none_of(
           std::array{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint,
                      vk::Format::eD24UnormS8Uint, vk::Format::eD16Unorm,
                      vk::Format::eD16UnormS8Uint},
           isResult))) {
    throw abcg::RuntimeError("Failed to find depth format");
  }

  return result.value();
}

void abcg::VulkanSwapchain::createDepthResources(
    VulkanSettings const &settings) {
  // auto hasStencilComponent{[](vk::Format format) {
  //   return format == vk::Format::eD32SfloatS8Uint ||
  //          format == vk::Format::eD24UnormS8Uint ||
  //          format == vk::Format::eD16UnormS8Uint;
  // }};

  auto const depthFormat{getDepthFormat(settings)};

  m_depthImage.create(
      m_device,
      {.info = {.imageType = vk::ImageType::e2D,
                .format = depthFormat,
                .extent = {.width = m_swapchainExtent.width,
                           .height = m_swapchainExtent.height,
                           .depth = 1},
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = m_device.getPhysicalDevice().getSampleCount(),
                .tiling = vk::ImageTiling::eOptimal,
                .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
                .sharingMode = vk::SharingMode::eExclusive,
                .initialLayout = vk::ImageLayout::eUndefined},
       .properties = vk::MemoryPropertyFlagBits::eDeviceLocal,
       .viewInfo = {
           .viewType = vk::ImageViewType::e2D,
           .format = depthFormat,
           .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eDepth,
                                .levelCount = 1,
                                .layerCount = 1}}});
}

void abcg::VulkanSwapchain::destroyDepthResources() { m_depthImage.destroy(); }

void abcg::VulkanSwapchain::createMSAAResources() {
  m_MSAAImage.create(
      m_device,
      {.info = {.imageType = vk::ImageType::e2D,
                .format = m_swapchainImageFormat,
                .extent = {.width = m_swapchainExtent.width,
                           .height = m_swapchainExtent.height,
                           .depth = 1},
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = m_device.getPhysicalDevice().getSampleCount(),
                .tiling = vk::ImageTiling::eOptimal,
                .usage = vk::ImageUsageFlagBits::eTransientAttachment |
                         vk::ImageUsageFlagBits::eColorAttachment,
                .sharingMode = vk::SharingMode::eExclusive,
                .initialLayout = vk::ImageLayout::eUndefined},
       .properties = vk::MemoryPropertyFlagBits::eDeviceLocal,
       .viewInfo = {
           .viewType = vk::ImageViewType::e2D,
           .format = m_swapchainImageFormat,
           .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                                .levelCount = 1,
                                .layerCount = 1}}});
}

void abcg::VulkanSwapchain::destroyMSAAResources() { m_MSAAImage.destroy(); }

void abcg::VulkanSwapchain::createRenderPasses(VulkanSettings const &settings) {
  std::vector<vk::AttachmentDescription> attachments;
  auto const &device{static_cast<vk::Device>(m_device)};
  auto const sampleCount{m_device.getPhysicalDevice().getSampleCount()};

  //
  // Main render pass
  //

  vk::AttachmentDescription colorAttachment{
      .format = m_swapchainImageFormat,
      .samples = sampleCount,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      // When multisampling is disabled, the image can be presented directly
      .finalLayout = sampleCount > vk::SampleCountFlagBits::e1
                         ? vk::ImageLayout::eColorAttachmentOptimal
                         : vk::ImageLayout::ePresentSrcKHR};
  attachments.push_back(colorAttachment);

  vk::AttachmentDescription depthAttachment{};
  if (settings.depthBufferSize > 0 || settings.stencilBufferSize > 0) {
    depthAttachment = vk::AttachmentDescription{
        .format = getDepthFormat(settings),
        .samples = sampleCount,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eDontCare, // Won't use after drawing
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal};
    attachments.push_back(depthAttachment);
  }

  auto attachmentCount{0U};

  vk::AttachmentReference const colorAttachmentRef{
      .attachment = attachmentCount++,
      .layout = vk::ImageLayout::eColorAttachmentOptimal};
  vk::AttachmentReference depthAttachmentRef{
      .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal};

  vk::SubpassDescription subpass{.pipelineBindPoint =
                                     vk::PipelineBindPoint::eGraphics,
                                 .colorAttachmentCount = 1,
                                 .pColorAttachments = &colorAttachmentRef};

  if (settings.depthBufferSize > 0 || settings.stencilBufferSize > 0) {
    depthAttachmentRef.attachment = attachmentCount++;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
  }

  vk::AttachmentDescription colorAttachmentResolve{};
  vk::AttachmentReference colorAttachmentResolveRef{};

  // If multisampling is used, we must include a resolve attachment
  if (sampleCount > vk::SampleCountFlagBits::e1) {
    colorAttachmentResolve = {.format = m_swapchainImageFormat,
                              .samples = vk::SampleCountFlagBits::e1,
                              .loadOp = vk::AttachmentLoadOp::eDontCare,
                              .storeOp = vk::AttachmentStoreOp::eStore,
                              .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
                              .stencilStoreOp =
                                  vk::AttachmentStoreOp::eDontCare,
                              .initialLayout = vk::ImageLayout::eUndefined,
                              .finalLayout = vk::ImageLayout::ePresentSrcKHR};

    colorAttachmentResolveRef = {.attachment = attachmentCount++,
                                 .layout =
                                     vk::ImageLayout::eColorAttachmentOptimal};

    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    attachments.push_back(colorAttachmentResolve);
  }

  vk::SubpassDependency dependency{
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
  };
  if (settings.depthBufferSize > 0 || settings.stencilBufferSize > 0) {
    // dependency.dstAccessMask |=
    //     vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests |
                              vk::PipelineStageFlagBits::eLateFragmentTests;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests |
                              vk::PipelineStageFlagBits::eLateFragmentTests;
    dependency.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    dependency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead |
                               vk::AccessFlagBits::eDepthStencilAttachmentWrite;
  } else {
    // .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
    //                 vk::PipelineStageFlagBits::eEarlyFragmentTests,
    // .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
    //                 vk::PipelineStageFlagBits::eEarlyFragmentTests,
    // .srcAccessMask = vk::AccessFlagBits::eNone,
    // .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlagBits::eNone;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
  }

  m_renderPassMain = device.createRenderPass(
      {.attachmentCount = gsl::narrow<uint32_t>(attachments.size()),
       .pAttachments = attachments.data(),
       .subpassCount = 1,
       .pSubpasses = &subpass,
       .dependencyCount = 1,
       .pDependencies = &dependency});

  //
  // UI render pass
  //

  attachments.clear();

  colorAttachment.loadOp = vk::AttachmentLoadOp::eLoad;
  // When multisampling is disabled, the image is presented directly in the
  // main render pass
  colorAttachment.initialLayout = sampleCount > vk::SampleCountFlagBits::e1
                                      ? vk::ImageLayout::eColorAttachmentOptimal
                                      : vk::ImageLayout::ePresentSrcKHR;
  attachments.push_back(colorAttachment);

  if (settings.depthBufferSize > 0 || settings.stencilBufferSize > 0) {
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    attachments.push_back(depthAttachment);
  }

  if (sampleCount > vk::SampleCountFlagBits::e1) {
    attachments.push_back(colorAttachmentResolve);
  }

  m_renderPassUI = device.createRenderPass(
      {.attachmentCount = gsl::narrow<uint32_t>(attachments.size()),
       .pAttachments = attachments.data(),
       .subpassCount = 1,
       .pSubpasses = &subpass,
       .dependencyCount = 1,
       .pDependencies = &dependency});
}

void abcg::VulkanSwapchain::destroyRenderPasses() {
  auto const &device{static_cast<vk::Device>(m_device)};
  device.destroyRenderPass(m_renderPassUI);
  device.destroyRenderPass(m_renderPassMain);
}

void abcg::VulkanSwapchain::createFramebuffers(VulkanSettings const &settings) {
  auto const &device{static_cast<vk::Device>(m_device)};
  auto const &queuesFamilies{m_device.getPhysicalDevice().getQueuesFamilies()};
  auto const sampleCount{m_device.getPhysicalDevice().getSampleCount()};

  if (!queuesFamilies.graphics.has_value()) {
    throw abcg::RuntimeError("Graphics queue family not found");
  }
  auto const graphicsQueueFamily{queuesFamilies.graphics.value()};

  for (auto &frame : m_frames) {
    // Each frame has its own transient graphics command pool
    frame.commandPool = device.createCommandPool(
        {.flags = vk::CommandPoolCreateFlagBits::eTransient,
         .queueFamilyIndex = graphicsQueueFamily});

    // Create a primary command buffer
    frame.commandBuffer =
        device
            .allocateCommandBuffers({.commandPool = frame.commandPool,
                                     .level = vk::CommandBufferLevel::ePrimary,
                                     .commandBufferCount = 1})
            .front();

    // Create a primary command buffer for the UI
    frame.commandBufferUI =
        device
            .allocateCommandBuffers({.commandPool = frame.commandPool,
                                     .level = vk::CommandBufferLevel::ePrimary,
                                     .commandBufferCount = 1})
            .front();

    // Create fence
    frame.fence =
        device.createFence({.flags = vk::FenceCreateFlagBits::eSignaled});

    // Set attachments
    std::vector<vk::ImageView> attachments{};
    if (sampleCount > vk::SampleCountFlagBits::e1) {
      // 0: Multisampled color buffer
      // 1: Multisampled depth buffer (optional)
      // 2: Resolved color buffer
      attachments.push_back(m_MSAAImage.getView());
      if (settings.depthBufferSize > 0 || settings.stencilBufferSize > 0) {
        attachments.push_back(m_depthImage.getView());
      }
      attachments.push_back(frame.colorImage.getView());
    } else {
      // 0: Color buffer
      // 1: Depth buffer (optional)
      attachments.push_back(frame.colorImage.getView());
      if (settings.depthBufferSize > 0 || settings.stencilBufferSize > 0) {
        attachments.push_back(m_depthImage.getView());
      }
    }

    // Create framebuffers
    frame.framebufferMain = device.createFramebuffer(
        {.renderPass = m_renderPassMain,
         .attachmentCount = gsl::narrow<uint32_t>(attachments.size()),
         .pAttachments = attachments.data(),
         .width = m_swapchainExtent.width,
         .height = m_swapchainExtent.height,
         .layers = 1});
  }

  // Create semaphores
  for (auto &frameSemaphore : m_frameSemaphores) {
    frameSemaphore.presentComplete = device.createSemaphore({});
    frameSemaphore.renderComplete = device.createSemaphore({});
  }
}
