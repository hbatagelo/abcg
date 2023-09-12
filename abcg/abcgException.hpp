/**
 * @file abcgException.hpp
 * @brief Header file of abcg::Exception and derived classes.
 *
 * Declaration of abcg::Exception and derived classes.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_EXCEPTION_HPP_
#define ABCG_EXCEPTION_HPP_

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
#if defined(__clang__)
#include <experimental/source_location>
namespace abcg {
// @cond Skipped by Doxygen
using source_location = std::experimental::source_location;
// @endcond
} // namespace abcg
#else
#include <source_location>
namespace abcg {
// @cond Skipped by Doxygen
using source_location = std::source_location;
// @endcond
} // namespace abcg
#endif
#endif

#include <stdexcept>
#include <string>

namespace abcg {
class Exception;
class RuntimeError;
class SDLError;
class SDLImageError;
} // namespace abcg

/**
 * @brief Base class for ABCg exception objects.
 *
 * Base exception class used for exception objects thrown by ABCg.
 */
class abcg::Exception : public std::exception {
public:
  explicit Exception(std::string_view what);

  [[nodiscard]] char const *what() const noexcept override;

private:
  std::string m_what{};
};

/**
 * @brief Represents an exception object for runtime errors.
 *
 * This is used for throwing exceptions for errors not related to OpenGL, SDL,
 * or SDL_image.
 *
 * The explanatory error message is appended with source location information.
 */
class abcg::RuntimeError : public abcg::Exception {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
public:
  explicit RuntimeError(
      std::string_view what,
      source_location const &sourceLocation = source_location::current());

private:
  static std::string prettyPrint(std::string_view what,
                                 source_location const &sourceLocation);
#else
public:
  explicit RuntimeError(std::string_view what);

private:
  static std::string prettyPrint(std::string_view what);
#endif
};

/**
 * @brief Represents an exception object for SDL errors.
 *
 * This is used for throwing exceptions for SDL errors that can be checked
 * with `SDL_GetError`.
 *
 * The explanatory error message is appended with source location information,
 * and the SDL error message.
 */
class abcg::SDLError : public abcg::Exception {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
public:
  explicit SDLError(
      std::string_view what,
      source_location const &sourceLocation = source_location::current());

private:
  static std::string prettyPrint(std::string_view what,
                                 source_location const &sourceLocation);
#else
public:
  explicit SDLError(std::string_view what);

private:
  static std::string prettyPrint(std::string_view what);
#endif
};

/**
 * @brief Represents an exception object for SDL_image errors.
 *
 * This is used for throwing exceptions for SDL errors that can be checked
 * with `IMG_GetError`.
 *
 * The explanatory error message is appended with source location information,
 * and the SDL_image error message.
 */
class abcg::SDLImageError : public abcg::Exception {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
public:
  explicit SDLImageError(
      std::string_view what,
      source_location const &sourceLocation = source_location::current());

private:
  static std::string prettyPrint(std::string_view what,
                                 source_location const &sourceLocation);
#else
public:
  explicit SDLImageError(std::string_view what);

private:
  static std::string prettyPrint(std::string_view what);
#endif
};

#endif
