/**
 * @file abcgOpenGLShader.hpp
 * @brief Declaration of helper functions for building OpenGL shaders.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_OPENGL_SHADER_HPP_
#define ABCG_OPENGL_SHADER_HPP_

#include "abcgOpenGLExternal.hpp"
#include "abcgShader.hpp"

#include <vector>

namespace abcg {
struct OpenGLShader;
}

/**
 * @brief OpenGL shader object and its corresponding stage.
 */
struct abcg::OpenGLShader {
  /** @brief Shader object. */
  GLuint shader{};
  /** @brief Shader stage (e.g., `GL_VERTEX_SHADER`). */
  GLuint stage{};
};

namespace abcg {
[[nodiscard]] GLuint
createOpenGLProgram(std::vector<ShaderSource> const &pathsOrSources,
                    bool throwOnError = true);
[[nodiscard]] std::vector<abcg::OpenGLShader>
triggerOpenGLShaderCompile(std::vector<ShaderSource> const &pathsOrSources);
bool checkOpenGLShaderCompile(std::vector<OpenGLShader> const &shaders,
                              bool throwOnError = true);
GLuint triggerOpenGLShaderLink(std::vector<OpenGLShader> const &shaders,
                               bool throwOnError = true);
bool checkOpenGLShaderLink(GLuint shaderProgram, bool throwOnError = true);
} // namespace abcg

#endif
