/**
 * @file abcg_image.cpp
 * @brief Definition of texture loading helper functions.
 *
 * This project is released under the MIT License.
 */

#include "abcg_image.hpp"

#include <fmt/core.h>

#include <cppitertools/itertools.hpp>
#include <fstream>
#include <gsl/gsl>
#include <span>
#include <vector>

#include "SDL_image.h"
#include "abcg_exception.hpp"
#include "abcg_external.hpp"

void flipHorizontally(gsl::not_null<SDL_Surface*> surface) {
  auto width{static_cast<size_t>(surface->w * surface->format->BytesPerPixel)};
  auto height{static_cast<size_t>(surface->h)};
  std::span pixels{static_cast<std::byte*>(surface->pixels), width * height};

  // Row of pixels for the swap
  std::vector<std::byte> pixelRow(width, std::byte{});

  // For each row
  for (auto rowIndex : iter::range(height)) {
    auto rowStart{width * rowIndex};
    auto rowEnd{rowStart + width - 1};
    // For each RGB triplet of this row
    // C++23: for (auto tripletStart : iter::range(0uz, width, 3uz)) {
    for (auto tripletStart : iter::range<std::size_t>(0, width, 3)) {
      pixelRow.at(tripletStart + 0) = pixels[rowEnd - tripletStart - 2];
      pixelRow.at(tripletStart + 1) = pixels[rowEnd - tripletStart - 1];
      pixelRow.at(tripletStart + 2) = pixels[rowEnd - tripletStart - 0];
    }
    memcpy(pixels.subspan(rowStart).data(), pixelRow.data(), width);
  }
}

void flipVertically(gsl::not_null<SDL_Surface*> surface) {
  auto width{static_cast<size_t>(surface->w * surface->format->BytesPerPixel)};
  auto height{static_cast<size_t>(surface->h)};
  std::span pixels{static_cast<std::byte*>(surface->pixels), width * height};

  // Row of pixels for the swap
  std::vector<std::byte> pixelRow(width, std::byte{});

  // If height is odd, don't need to swap middle row
  size_t halfHeight{height / 2};
  for (auto rowIndex : iter::range(halfHeight)) {
    auto rowStartFromTop{width * rowIndex};
    auto rowStartFromBottom{width * (height - rowIndex - 1)};
    memcpy(pixelRow.data(), pixels.subspan(rowStartFromTop).data(), width);
    memcpy(pixels.subspan(rowStartFromTop).data(),
           pixels.subspan(rowStartFromBottom).data(), width);
    memcpy(pixels.subspan(rowStartFromBottom).data(), pixelRow.data(), width);
  }
}

GLuint abcg::opengl::loadTexture(std::string_view path, bool generateMipmaps) {
  GLuint textureID{};

  // Copy file data into buffer
  std::ifstream input(path.data(), std::ios::binary);
  if (!input) {
    throw abcg::Exception{abcg::Exception::Runtime(
        fmt::format("Failed to open texture file {}", path))};
  }
  std::vector<char> buffer((std::istreambuf_iterator<char>(input)),
                           std::istreambuf_iterator<char>());

  // Load the bitmap
  if (SDL_Surface * surface{IMG_Load(path.data())}) {
    // Enforce RGB/RGBA
    GLenum format{0};
    SDL_Surface* formattedSurface{nullptr};
    if (surface->format->BytesPerPixel == 3) {
      formattedSurface =
          SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB24, 0);
      format = GL_RGB;
    } else {
      formattedSurface =
          SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
      format = GL_RGBA;
    }
    SDL_FreeSurface(surface);

    // Flip upside down
    flipVertically(formattedSurface);

    // Generate the texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format),
                 formattedSurface->w, formattedSurface->h, 0, format,
                 GL_UNSIGNED_BYTE, formattedSurface->pixels);

    SDL_FreeSurface(formattedSurface);

    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate the mipmap levels
    if (generateMipmaps) {
      glGenerateMipmap(GL_TEXTURE_2D);

      // Override minifying filtering
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
    }

    // Set texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  } else {
    throw abcg::Exception{abcg::Exception::Runtime(
        fmt::format("Failed to load texture file {}", path))};
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  return textureID;
}

GLuint abcg::opengl::loadCubemap(std::array<std::string_view, 6> paths,
                                 bool generateMipmaps, bool rightHandedSystem) {
  GLuint textureID{};
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  for (auto&& [index, path] : iter::enumerate(paths)) {
    // Copy file data into buffer
    std::ifstream input(path.data(), std::ios::binary);
    if (!input) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to open texture file {}", path))};
    }
    std::vector<char> buffer((std::istreambuf_iterator<char>(input)),
                             std::istreambuf_iterator<char>());

    // Load the bitmap
    if (SDL_Surface * surface{IMG_Load(path.data())}) {
      // Enforce RGB
      SDL_Surface* formattedSurface{
          SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB24, 0)};
      SDL_FreeSurface(surface);

      auto target{GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(index)};

      // LHS to RHS
      if (rightHandedSystem) {
        if (target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y) {
          // Flip upside down
          flipVertically(formattedSurface);
        } else {
          flipHorizontally(formattedSurface);
        }

        // Swap -z with +z
        if (target == GL_TEXTURE_CUBE_MAP_POSITIVE_Z)
          target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        else if (target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
          target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
      }

      // Create texture
      glTexImage2D(target, 0, GL_RGB, formattedSurface->w, formattedSurface->h,
                   0, GL_RGB, GL_UNSIGNED_BYTE, formattedSurface->pixels);

      SDL_FreeSurface(formattedSurface);
    } else {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load texture file {}", path))};
    }
  }

  // Set texture wrapping
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Set texture filtering
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Generate the mipmap levels
  if (generateMipmaps) {
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Override minifying filtering
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
  }

  return textureID;
}
