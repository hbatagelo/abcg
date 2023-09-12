/**
 * @file abcgOpenGLExternal.hpp
 * @brief Header file for including OpenGL-related third-party dependencies.
 *
 * Preprocessor definitions and includes of OpenGL-related third-party
 * dependencies.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_OPENGL_EXTERNAL_HPP_
#define ABCG_OPENGL_EXTERNAL_HPP_

// @cond Skipped by Doxygen

#if !defined(__EMSCRIPTEN__)
#if !defined(GLEW_STATIC)
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#endif

#if defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>

#include "SDL_opengles2.h"
#include "emscripten.h"
#include "emscripten/html5.h"
#endif

// @endcond

#endif
