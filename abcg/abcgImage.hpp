/**
 * @file abcgImage.hpp
 * @brief Declaration of image manipulation helper functions.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_IMAGE_HPP_
#define ABCG_IMAGE_HPP_

#include <SDL_image.h>

namespace abcg {
void flipHorizontally(SDL_Surface &surface);
void flipVertically(SDL_Surface &surface);
} // namespace abcg

#endif
