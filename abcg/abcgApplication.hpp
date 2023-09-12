/**
 * @file abcgApplication.hpp
 * @brief Header file of abcg::Application.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_APPLICATION_HPP_
#define ABCG_APPLICATION_HPP_

#include <string>

#define ABCG_VERSION_MAJOR 3
#define ABCG_VERSION_MINOR 1
#define ABCG_VERSION_PATCH 0

/**
 * @brief Root namespace.
 */
namespace abcg {
class Application;
class Window;
#if defined(__EMSCRIPTEN__)
void mainLoopCallback(void *userData);
#endif
} // namespace abcg

/**
 * @brief Manages the application's control flow.
 *
 * This is the class that starts an ABCg application, initializes the SDL
 * modules and enters the main event loop.
 */
class abcg::Application {
public:
  Application(int argc, char **argv);

  void run(Window &window);

  static std::string const &getAssetsPath() noexcept;
  static std::string const &getBasePath() noexcept;

private:
  void mainLoopIterator(bool &done) const;

  Window *m_window{};

#if defined(__EMSCRIPTEN__)
  friend void mainLoopCallback(void *userData);
#endif

  // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
  // See https://bugs.llvm.org/show_bug.cgi?id=48040
  static inline std::string m_assetsPath;
  static inline std::string m_basePath;
  // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
};

#endif
