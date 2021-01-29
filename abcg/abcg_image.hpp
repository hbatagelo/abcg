/**
 * @file abcg_image.hpp
 * @brief Declaration of texture loading helper functions.
 *
 * This project is released under the MIT License.
 */

#ifndef ABCG_IMAGE_HPP_
#define ABCG_IMAGE_HPP_

#include <abcg_external.hpp>
#include <array>
#include <string_view>

namespace abcg::opengl {
[[nodiscard]] GLuint loadTexture(std::string_view path,
                                 bool generateMipmaps = true);
[[nodiscard]] GLuint loadCubemap(std::array<std::string_view, 6> paths,
                                 bool generateMipmaps = true);
}  // namespace abcg::opengl

#endif