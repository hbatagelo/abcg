/**
 * @file abcgVulkanPipeline.hpp
 * @brief Header file of abcg::VulkanPipeline
 *
 * Declaration of abcg::VulkanPipeline.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_VULKAN_PIPELINE_HPP_
#define ABCG_VULKAN_PIPELINE_HPP_

#include "abcgVulkanShader.hpp"
#include "abcgVulkanSwapchain.hpp"

namespace abcg {
struct VulkanPipelineCreateInfo;
class VulkanPipeline;
} // namespace abcg

/**
 * @brief Creation info structure for abcg::VulkanPipeline::create.
 */
struct abcg::VulkanPipelineCreateInfo {
  std::vector<abcg::VulkanShader> shaders{};
  std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};
  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{};
  std::optional<std::vector<vk::Viewport>> viewports{};
  std::optional<std::vector<vk::Rect2D>> scissors{};
  vk::PipelineRasterizationStateCreateInfo rasterizationState{};
  std::optional<vk::PipelineMultisampleStateCreateInfo> multisampleState{};
  std::optional<vk::PipelineDepthStencilStateCreateInfo> depthStencilState{};
  std::optional<vk::PipelineColorBlendAttachmentState> colorBlendAttachment{};
  std::optional<vk::PipelineColorBlendStateCreateInfo> colorBlendState{};
  std::vector<vk::DynamicState> dynamicStates{};
  vk::PipelineLayoutCreateInfo pipelineLayout{};
  vk::PipelineCache pipelineCache{};
};

/**
 * @brief A class for representing a Vulkan pipeline.
 *
 * This class provides helper functions for creating and managing vk::Pipeline
 * objects.
 */
class abcg::VulkanPipeline {
public:
  void create(VulkanSwapchain const &swapchain,
              VulkanPipelineCreateInfo const &createInfo);
  void destroy();

  explicit operator vk::Pipeline const &() const noexcept;

  [[nodiscard]] vk::PipelineLayout const &getLayout() const noexcept;

private:
  vk::Pipeline m_pipeline;
  vk::PipelineLayout m_pipelineLayout;
  vk::Device m_device;
};

#endif