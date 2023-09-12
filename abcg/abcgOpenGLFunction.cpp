/**
 * @file abcgOpenGLFunction.cpp
 * @brief Definition of OpenGL-related error checking functions.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgOpenGLFunction.hpp"
#include "abcgOpenGLError.hpp"

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
/**
 * @brief Checks OpenGL error status and throws on error with a log message.
 *
 * @param sourceLocation Information about the source code, to be used for
 * logging.
 * @param appendString A string to be appended to "OpenGL error " in the
 * exception explanatory string.
 *
 * @throw abcg::Exception::OpenGLError.
 */
void abcg::checkGLError(source_location const &sourceLocation,
                        std::string_view const appendString) {
  // Throw on first error
  if (auto const status{glGetError()}; status != GL_NO_ERROR) {
    throw abcg::OpenGLError(appendString, status, sourceLocation);
  }
}
#endif
