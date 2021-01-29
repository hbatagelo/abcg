/**
 * @file abcg_exception.hpp
 * @brief abcg::Exception header file.
 *
 * Declaration of abcg::Exception class.
 *
 * This project is released under the MIT License.
 */

#ifndef ABCG_EXCEPTION_HPP_
#define ABCG_EXCEPTION_HPP_

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
#include <experimental/source_location>
#endif

#include <stdexcept>
#include <string>

namespace abcg {
class Exception;
}  // namespace abcg

/**
 * @brief abcg::Exception class.
 *
 */
class abcg::Exception : public std::runtime_error {
 public:
  explicit Exception(std::string_view what);

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
  static std::string Runtime(
      std::string_view what,
      const std::experimental::source_location& sourceLocation =
          std::experimental::source_location::current());

  static std::string OpenGL(
      std::string_view prefix, unsigned int error,
      const std::experimental::source_location& sourceLocation =
          std::experimental::source_location::current());

  static std::string SDL(
      std::string_view what,
      const std::experimental::source_location& sourceLocation =
          std::experimental::source_location::current());

  static std::string SDLImage(
      std::string_view what,
      const std::experimental::source_location& sourceLocation =
          std::experimental::source_location::current());
#else
  static std::string Runtime(std::string_view what);
  static std::string OpenGL(std::string_view prefix, unsigned int error);
  static std::string SDL(std::string_view what);
  static std::string SDLImage(std::string_view what);
#endif

 private:
  constexpr static const auto codeRed{"\033[0;31m"};
  constexpr static const auto codeReset{"\033[0m"};

  static std::string_view getGLErrorString(unsigned int error);
};

#endif
