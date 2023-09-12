/**
 * @file abcgOpenGLShader.cpp
 * @brief Definition of helper functions for building OpenGL shaders.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgOpenGLShader.hpp"

#include <cppitertools/itertools.hpp>
#include <fmt/core.h>
#include <gsl/gsl>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

#include "abcgException.hpp"

namespace {
void printShaderInfoLog(GLuint const shader, std::string_view prefix) {
  GLint infoLogLength{};
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (infoLogLength > 0) {
    std::vector<GLchar> infoLog{};
    infoLog.reserve(gsl::narrow<std::size_t>(infoLogLength));
    glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());
    fmt::print("Shader information log ({} shader):\n{}\n", prefix,
               infoLog.data());
  }
}

void printProgramInfoLog(GLuint const program) {
  GLint infoLogLength{};
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (infoLogLength > 0) {
    std::vector<GLchar> infoLog{};
    infoLog.reserve(gsl::narrow<std::size_t>(infoLogLength));
    glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());
    fmt::print("Program information log:\n{}\n", infoLog.data());
  }
}

[[nodiscard]] char const *shaderStageToText(GLuint stage) {
  switch (stage) {
  case GL_VERTEX_SHADER:
    return "vertex";
  case GL_FRAGMENT_SHADER:
    return "fragment";
#if !defined(__EMSCRIPTEN__)
  case GL_GEOMETRY_SHADER:
    return "geometry";
  case GL_TESS_CONTROL_SHADER:
    return "tess control";
  case GL_TESS_EVALUATION_SHADER:
    return "tess evaluation";
  case GL_COMPUTE_SHADER:
    return "compute";
#if defined(GL_TASK_SHADER_NV)
  case GL_TASK_SHADER_NV:
    return "task";
#endif
#if defined(GL_MESH_SHADER_NV)
  case GL_MESH_SHADER_NV:
    return "mesh";
#endif
#endif
  default:
    return "unknown";
  }
}

// If filenameOrText is a filename, returns the contents of the file (assumed
// to be in text format). Otherwise, returns filenameOrText.
[[nodiscard]] std::string toSource(std::string_view filenameOrText) {
  static const std::size_t maxPathSize{260};
  if (filenameOrText.size() > maxPathSize ||
      !std::filesystem::exists(filenameOrText)) {
    return filenameOrText.data();
  }
  std::stringstream source;
  if (std::ifstream stream(filenameOrText.data()); stream) {
    source << stream.rdbuf();
    stream.close();
  } else {
    throw abcg::RuntimeError(
        fmt::format("Failed to read file {}", filenameOrText));
  }
  return source.str();
}

// Compiles a shader and returns immediately (i.e. don't wait until completion).
// Returns the shader ID of the compiled shader.
[[nodiscard]] abcg::OpenGLShader compileHelper(std::string_view shaderSource,
                                               GLuint shaderStage) {
  auto shaderID{glCreateShader(shaderStage)};
  auto const *source{shaderSource.data()};
  glShaderSource(shaderID, 1, &source, nullptr);
  glCompileShader(shaderID);
  return {shaderID, shaderStage};
}

// Deletes the OpenGL shader objects in `shaderIDs`
void deleteShaders(std::vector<abcg::OpenGLShader> const &shaderIDs) {
  for (auto const &shaderID : shaderIDs) {
    glDeleteShader(shaderID.shader);
  }
}

[[nodiscard]] GLuint abcgStageToOpenGLStage(abcg::ShaderStage stage) {
  switch (stage) {
  case abcg::ShaderStage::Vertex:
    return GL_VERTEX_SHADER;
  case abcg::ShaderStage::Fragment:
    return GL_FRAGMENT_SHADER;
#if !defined(__EMSCRIPTEN__)
  case abcg::ShaderStage::TessellationControl:
    return GL_TESS_CONTROL_SHADER;
  case abcg::ShaderStage::TessellationEvaluation:
    return GL_TESS_EVALUATION_SHADER;
  case abcg::ShaderStage::Geometry:
    return GL_GEOMETRY_SHADER;
  case abcg::ShaderStage::Compute:
    return GL_COMPUTE_SHADER;
#if defined(GL_TASK_SHADER_NV)
  case abcg::ShaderStage::Task:
    return GL_TASK_SHADER_NV;
#endif
#if defined(GL_MESH_SHADER_NV)
  case abcg::ShaderStage::Mesh:
    return GL_MESH_SHADER_NV;
#endif
#endif
  default:
    throw abcg::RuntimeError("Unknown shader stage");
  }
}
} // namespace

/**
 * @brief Creates a program object from a group of shader paths or source codes.
 *
 * @param pathsOrSources Paths or source codes of the shaders to be compiled and
 * linked to the program.
 * @param throwOnError Whether to throw exceptions on compile/link errors.
 *
 * @throw abcg::RuntimeError if the shader could not be read from file, or if
 * the program could not be created, or if the compilation of any shader has
 * failed, or if the linking has failed.
 *
 * @return ID of the program object, or 0 on error.
 */
GLuint
abcg::createOpenGLProgram(std::vector<ShaderSource> const &pathsOrSources,
                          bool throwOnError) {
  std::vector<ShaderSource> sources;
  sources.reserve(pathsOrSources.size());
  for (auto const &pathOrSource : pathsOrSources) {
    sources.push_back(
        {.source = toSource(pathOrSource.source), .stage = pathOrSource.stage});
  }

  std::vector<OpenGLShader> compiledShaders;
  compiledShaders.reserve(sources.size());
  for (auto const &source : sources) {
    compiledShaders.push_back(
        compileHelper(source.source, abcgStageToOpenGLStage(source.stage)));
  }

  if (!checkOpenGLShaderCompile(compiledShaders, throwOnError))
    return 0U;

  auto const shaderProgram{glCreateProgram()};
  if (shaderProgram == 0) {
    deleteShaders(compiledShaders);
    if (throwOnError) {
      throw abcg::RuntimeError("Failed to create program");
    }
    return 0;
  }

  for (auto const &shader : compiledShaders) {
    glAttachShader(shaderProgram, shader.shader);
  }

  glLinkProgram(shaderProgram);

  for (auto const &shader : compiledShaders) {
    glDetachShader(shaderProgram, shader.shader);
    glDeleteShader(shader.shader);
  }

  GLint linkStatus{};
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) {
    if (throwOnError) {
      fmt::print("\n");
      printProgramInfoLog(shaderProgram);
      glDeleteProgram(shaderProgram);
      throw abcg::RuntimeError("Failed to link program");
    }
    glDeleteProgram(shaderProgram);
    return 0U;
  }

  return shaderProgram;
}

/**
 * @brief Triggers the compilation of a group of shaders and returns
 * immediately.
 *
 * This function immediately returns the shader objects without querying the
 * compile status. The compilation may still be running in parallel. The compile
 * status must be checked later with abcg::checkCompile.
 *
 * This is an alternative to abcg::createOpenGLProgram to prevent halting the
 * application when building complex programs. Instead of waiting for completion
 * of the build process, the user user can call
 * abcg::triggerOpenGLShaderCompile, followed by abcg::checkOpenGLShaderCompile,
 * abcg::triggerOpenGLShaderLink, and abcg::checkOpenGLShaderLink, interleaved
 * with other processing tasks.
 *
 * @param pathsOrSources Paths or source codes of the shaders to be compiled.
 *
 * @throw abcg::RuntimeError if the shader could not be read from file.
 *
 * @return Container of shader objects being compiled.
 *
 * @sa abcg::checkOpenGLShaderCompile.
 */
std::vector<abcg::OpenGLShader> abcg::triggerOpenGLShaderCompile(
    std::vector<ShaderSource> const &pathsOrSources) {
  std::vector<ShaderSource> sources;
  sources.reserve(pathsOrSources.size());
  for (auto const &pathOrSource : pathsOrSources) {
    sources.push_back(
        {.source = toSource(pathOrSource.source), .stage = pathOrSource.stage});
  }

  std::vector<OpenGLShader> compiledShaders;
  compiledShaders.reserve(sources.size());
  for (auto const &source : sources) {
    compiledShaders.push_back(
        compileHelper(source.source, abcgStageToOpenGLStage(source.stage)));
  }

  return compiledShaders;
}

/**
 * @brief Queries the compile status of shader objects.
 *
 * This should be called after abcg::triggerOpenGLShaderCompile. The function
 * waits until all shaders are compiled.
 *
 * @param shaders Shader objects returned by abcg::triggerOpenGLShaderCompile.
 * @param throwOnError Whether to throw exceptions on compile errors.
 *
 * @throw abcg::RuntimeError if the compilation of any shader failed.
 *
 * @return `true` if the shaders compiled with success; `false` otherwise.
 *
 * @sa abcg::triggerOpenGLShaderLink.
 */
bool abcg::checkOpenGLShaderCompile(std::vector<OpenGLShader> const &shaders,
                                    bool throwOnError) {
  // Check compile status
  for (auto &&[index, shader] : iter::enumerate(shaders)) {
    GLint compileStatus{};
    glGetShaderiv(shader.shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
      if (throwOnError) {
        fmt::print("\n");
        auto const *shaderStage{shaderStageToText(shader.stage)};
        printShaderInfoLog(shader.shader, shaderStage);
        deleteShaders(shaders);
        throw abcg::RuntimeError(
            fmt::format("Failed to compile {} shader", shaderStage));
      }
      deleteShaders(shaders);
      return false;
    }
  }
  return true;
}

/**
 * @brief Triggers the linking of a group of shader objects.
 *
 * This should be called after abcg::checkOpenGLShaderCompile. The function will
 * return immediately.
 *
 * @param shaders Shader objects returned by abcg::triggerOpenGLShaderCompile.
 * @param throwOnError Whether to throw exceptions on program creation error.
 *
 * @throw abcg::RuntimeError if the program could not be created.
 *
 * @return ID of the program object with the shaders attached, or 0 on error.
 *
 * @sa abcg::checkOpenGLShaderLink.
 */
GLuint abcg::triggerOpenGLShaderLink(std::vector<OpenGLShader> const &shaders,
                                     bool throwOnError) {
  auto const shaderProgram{glCreateProgram()};
  if (shaderProgram == 0) {
    deleteShaders(shaders);
    if (throwOnError) {
      throw abcg::RuntimeError("Failed to create program");
    }
    return 0;
  }

  for (auto const &shader : shaders) {
    glAttachShader(shaderProgram, shader.shader);
  }

  glLinkProgram(shaderProgram);

  for (auto const &shader : shaders) {
    glDetachShader(shaderProgram, shader.shader);
  }

  deleteShaders(shaders);

  return shaderProgram;
}

/**
 * @brief Queries the link status of the shaders attached to a program object.
 *
 * This should be called after abcg::triggerOpenGLShaderLink. The function will
 * wait until all shaders are linked.
 *
 * @param shaderProgram ID of the shader program returned by
 * abcg::triggerOpenGLShaderLink.
 * @param throwOnError Whether to throw exceptions on link error.
 *
 * @throw abcg::RuntimeError if program linking failed.
 *
 * @return `true` if the shaders linked with success; `false` otherwise.
 */
bool abcg::checkOpenGLShaderLink(GLuint shaderProgram, bool throwOnError) {
  GLint linkStatus{};
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) {
    if (throwOnError) {
      fmt::print("\n");
      printProgramInfoLog(shaderProgram);
      glDeleteProgram(shaderProgram);
      throw abcg::RuntimeError("Failed to link program");
    }
    glDeleteProgram(shaderProgram);
    return false;
  }

  return true;
}