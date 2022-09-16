/**
 * @file abcgImage.hpp
 * @brief Declaration of texture loading helper functions.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2022 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_IMAGE_HPP_
#define ABCG_IMAGE_HPP_

#include <SDL_image.h>
#include <gsl/pointers>

namespace abcg {
void flipHorizontally(gsl::not_null<SDL_Surface *> surface);
void flipVertically(gsl::not_null<SDL_Surface *> surface);
} // namespace abcg

#endif
