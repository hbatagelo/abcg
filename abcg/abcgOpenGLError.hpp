/**
 * @file abcgOpenGLError.hpp
 * @brief Header file of abcg::OpenGLError and derived classes.
 *
 * Declaration of abcg::OpenGLError and derived classes.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_OPENGL_ERROR_HPP_
#define ABCG_OPENGL_ERROR_HPP_

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
#if defined(__clang__)
#include <experimental/source_location>
namespace abcg {
using source_location = std::experimental::source_location;
} // namespace abcg
#else
#include <source_location>
namespace abcg {
using source_location = std::source_location;
} // namespace abcg
#endif
#endif

#include "abcgException.hpp"

namespace abcg {
class OpenGLError;
} // namespace abcg

/**
 * @brief Represents an exception object for OpenGL errors.
 *
 * This is used for throwing exceptions for OpenGL errors that can be checked
 * with `glGetError`.
 *
 * The explanatory error message is appended with source location information,
 * and descriptive messages regarding the GL error codes.
 */
class abcg::OpenGLError : public abcg::Exception {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
public:
  explicit OpenGLError(
      std::string_view what, unsigned int errorCode,
      source_location const &sourceLocation = source_location::current());

private:
  static std::string prettyPrint(std::string_view what, unsigned int errorCode,
                                 source_location const &sourceLocation);
#else
public:
  explicit OpenGLError(std::string_view what, unsigned int errorCode);

private:
  static std::string prettyPrint(std::string_view what, unsigned int errorCode);
#endif

  static std::string_view getGLErrorString(unsigned int errorCode);
};

#endif
