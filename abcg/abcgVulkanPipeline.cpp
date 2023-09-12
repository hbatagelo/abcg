/**
 * @file abcgVulkanPipeline.cpp
 * @brief Definition of abcg::VulkanPipeline
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgVulkanPipeline.hpp"

#include <gsl/gsl>

void abcg::VulkanPipeline::create(VulkanSwapchain const &swapchain,
                                  VulkanPipelineCreateInfo const &createInfo) {
  m_device = static_cast<vk::Device>(swapchain.getDevice());
  auto const &physicalDevice{swapchain.getDevice().getPhysicalDevice()};

  // Shader stages
  std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
  shaderStages.reserve(createInfo.shaders.size());
  for (auto const &shader : createInfo.shaders) {
    shaderStages.push_back({.stage = shader.getStage(),
                            .module = shader.getModule(),
                            .pName = "main"});
  }

  // Vertex binding and attributes
  vk::PipelineVertexInputStateCreateInfo const vertexInputState{
      .vertexBindingDescriptionCount =
          gsl::narrow<uint32_t>(createInfo.bindingDescriptions.size()),
      .pVertexBindingDescriptions = createInfo.bindingDescriptions.data(),
      .vertexAttributeDescriptionCount =
          gsl::narrow<uint32_t>(createInfo.attributeDescriptions.size()),
      .pVertexAttributeDescriptions = createInfo.attributeDescriptions.data()};

  // Viewport state
  auto viewports{createInfo.viewports.value_or(std::vector<vk::Viewport>{
      {.width = gsl::narrow<float>(swapchain.getExtent().width),
       .height = gsl::narrow<float>(swapchain.getExtent().height),
       .minDepth = 0,
       .maxDepth = 1}})};
  auto scissors{createInfo.scissors.value_or(
      std::vector<vk::Rect2D>{{.extent = swapchain.getExtent()}})};
  vk::PipelineViewportStateCreateInfo const viewportState{
      .viewportCount = gsl::narrow<uint32_t>(viewports.size()),
      .pViewports = viewports.data(),
      .scissorCount = gsl::narrow<uint32_t>(scissors.size()),
      .pScissors = scissors.data()};

  // Multisampling
  vk::PipelineMultisampleStateCreateInfo multisampleState{};
  if (createInfo.multisampleState.has_value()) {
    multisampleState = createInfo.multisampleState.value();
  } else {
    auto sampleCount{physicalDevice.getSampleCount()};
    multisampleState.rasterizationSamples = sampleCount;
    if (sampleCount > vk::SampleCountFlagBits::e1) {
      // Enable sample shading if available
      if (static_cast<vk::PhysicalDevice>(physicalDevice)
              .getFeatures()
              .sampleRateShading == VK_TRUE) {
        multisampleState.sampleShadingEnable = VK_TRUE;
        multisampleState.minSampleShading = 0.5f;
      }
    }
  }

  // Depth and stencil
  vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
  if (createInfo.depthStencilState.has_value()) {
    depthStencilState = createInfo.depthStencilState.value();
  } else {
    if (static_cast<vk::Image>(swapchain.getDepthImage())) {
      depthStencilState = {.depthTestEnable = VK_TRUE,
                           .depthWriteEnable = VK_TRUE,
                           .depthCompareOp = vk::CompareOp::eLess};
    }
  }

  // Color blending
  auto colorBlendAttachment{createInfo.colorBlendAttachment.value_or(
      vk::PipelineColorBlendAttachmentState{
          .blendEnable = VK_FALSE,
          .colorWriteMask = vk::ColorComponentFlagBits::eR |
                            vk::ColorComponentFlagBits::eG |
                            vk::ColorComponentFlagBits::eB |
                            vk::ColorComponentFlagBits::eA})};
  auto colorBlendState{
      createInfo.colorBlendState.value_or(vk::PipelineColorBlendStateCreateInfo{
          .logicOpEnable = VK_FALSE,
          .attachmentCount = 1,
          .pAttachments = &colorBlendAttachment})};

  // Dynamic state
  vk::PipelineDynamicStateCreateInfo const dynamicState{
      .dynamicStateCount =
          gsl::narrow<uint32_t>(createInfo.dynamicStates.size()),
      .pDynamicStates = createInfo.dynamicStates.data()};

  // Pipeline layout
  m_pipelineLayout = m_device.createPipelineLayout(createInfo.pipelineLayout);

  vk::GraphicsPipelineCreateInfo const pipelineCreateInfo{
      .stageCount = gsl::narrow<uint32_t>(shaderStages.size()),
      .pStages = shaderStages.data(),
      .pVertexInputState = &vertexInputState,
      .pInputAssemblyState = &createInfo.inputAssemblyState,
      .pViewportState = &viewportState,
      .pRasterizationState = &createInfo.rasterizationState,
      .pMultisampleState = &multisampleState,
      .pDepthStencilState = &depthStencilState,
      .pColorBlendState = &colorBlendState,
      .pDynamicState = &dynamicState,
      .layout = m_pipelineLayout,
      .renderPass = swapchain.getMainRenderPass(),
      .subpass = 0
      // .basePipelineHandle = VK_NULL_HANDLE,
      // .basePipelineIndex = -1
  };

  auto result{m_device.createGraphicsPipeline(createInfo.pipelineCache,
                                              pipelineCreateInfo)};
  m_pipeline = result.value;
}

void abcg::VulkanPipeline::destroy() {
  if (!m_device) {
    return;
  }

  m_device.waitIdle();
  m_device.destroyPipeline(m_pipeline);
  m_device.destroyPipelineLayout(m_pipelineLayout);
}

/**
 * @brief Conversion to vk::Pipeline.
 */
abcg::VulkanPipeline::operator vk::Pipeline const &() const noexcept {
  return m_pipeline;
}

/**
 * @brief Access to vk::PipelineLayout.
 *
 * @return Pipeline layout of the pipeline.
 */
vk::PipelineLayout const &abcg::VulkanPipeline::getLayout() const noexcept {
  return m_pipelineLayout;
}
