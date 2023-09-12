/**
 * @file abcgVulkanShader.hpp
 * @brief Declaration of helper functions for creating Vulkan shaders.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_VULKAN_SHADER_HPP_
#define ABCG_VULKAN_SHADER_HPP_

#include "abcgShader.hpp"
#include "abcgVulkanDevice.hpp"

namespace abcg {
class VulkanShader;
} // namespace abcg

/**
 * @brief A class for representing a Vulkan shader.
 *
 * This class compiles a GLSL shader into a Vulkan SPIR-V shader and creates the
 * corresponding vk::ShaderModule.
 */
class abcg::VulkanShader {
public:
  void create(VulkanDevice const &device, ShaderSource const &pathOrSource);
  void destroy();

  [[nodiscard]] vk::ShaderStageFlagBits const &getStage() const noexcept;
  [[nodiscard]] vk::ShaderModule const &getModule() const noexcept;

private:
  vk::ShaderStageFlagBits m_stage{};
  vk::ShaderModule m_module;
  vk::Device m_device;
};

#endif
