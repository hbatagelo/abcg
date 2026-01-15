/**
 * @file abcgOpenGLImage.hpp
 * @brief Declaration of OpenGL texture loading helper functions.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2026 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_OPENGL_IMAGE_HPP_
#define ABCG_OPENGL_IMAGE_HPP_

#include "abcgOpenGLExternal.hpp"

#include <array>
#include <string>

namespace abcg {
struct OpenGLTextureCreateInfo;
struct OpenGLCubemapCreateInfo;

[[nodiscard]] GLuint
loadOpenGLTexture(OpenGLTextureCreateInfo const &createInfo);
[[nodiscard]] GLuint
loadOpenGLCubemap(OpenGLCubemapCreateInfo const &createInfo);
} // namespace abcg

/**
 * @brief Configuration settings for creating a 2D texture for OpenGL.
 */
struct abcg::OpenGLTextureCreateInfo {
  /** @brief Path to the image file (PNG or JPEG). */
  std::string path;
  /** @brief Whether to generate mipmap levels. */
  bool generateMipmaps{true};
  /** @brief Whether to flip the image upside down. */
  bool flipUpsideDown{true};
  /** @brief Whether to apply gamma decoding (expansion) to convert an image in
   * sRGB space to linear space. */
  bool sRGBToLinear{false};
};

/**
 * @brief Configuration settings for creating a cubemap texture for OpenGL.
 */
struct abcg::OpenGLCubemapCreateInfo {
  /** @brief Array of paths to the image files (PNG or JPEG) containing the
   * sides of the cube map, given in the order +x, -y, +y, -y, +z, -z. */
  std::array<std::string, 6> paths{};
  /** @brief Whether to generate mipmap levels. */
  bool generateMipmaps{true};
  /** @brief Whether to convert the cubemap from a left-handed system to a
   * right-handed system. */
  bool rightHandedSystem{true};
};

#endif
