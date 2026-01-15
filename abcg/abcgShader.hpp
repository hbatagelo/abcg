/**
 * @file abcgShader.hpp
 * @brief Declaration of a structure for building shaders.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2026 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_SHADER_HPP_
#define ABCG_SHADER_HPP_

#include <cstdint>
#include <string>

namespace abcg {
struct ShaderSource;
enum class ShaderStage : std::uint8_t;
} // namespace abcg

/**
 * @brief Enumeration of shader stages.
 *
 */
enum class abcg::ShaderStage : std::uint8_t {
  Vertex,
  TessellationControl,
  TessellationEvaluation,
  Geometry,
  Fragment,
  Compute,
  RayGen,
  AnyHit,
  ClosestHit,
  Miss,
  Intersection,
  Callable,
  Task,
  Mesh
};

/**
 * @brief Shader source code and corresponding stage.
 */
struct abcg::ShaderSource {
  /** @brief Shader source code.
   *
   * This can be either the path to the shader file or the string containing the
   * source code.
   */
  std::string source;
  /** @brief Shader stage. */
  abcg::ShaderStage stage{};
};

#endif
