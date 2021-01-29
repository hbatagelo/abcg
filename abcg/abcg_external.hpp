/**
 * @file abcg_external.hpp
 * @brief Header file for including external dependencies.
 *
 * Preprocessor definitions and includes of external dependencies.
 *
 * This project is released under the MIT License.
 */

#ifndef ABCG_EXTERNAL_HPP_
#define ABCG_EXTERNAL_HPP_

#define GLM_ENABLE_EXPERIMENTAL

#if !defined(__EMSCRIPTEN__)
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <SDL.h>

#if defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>

#include "SDL_opengles2.h"
#include "emscripten.h"
#include "emscripten/html5.h"
#endif

#endif
