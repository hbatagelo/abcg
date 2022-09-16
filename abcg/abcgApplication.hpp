/**
 * @file abcgApplication.hpp
 * @brief Header file of abcg::Application.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2022 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_APPLICATION_HPP_
#define ABCG_APPLICATION_HPP_

#include <string>

#define ABCG_VERSION_MAJOR 3
#define ABCG_VERSION_MINOR 0
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

  /**
   * @brief Returns the path to the application's assets directory, relative to
   * the directory the executable is launched from.
   *
   * @return Path to the application's `assets` directory, relative to the
   * location from which the application was launched. For example, the assets
   * path will be `./app/assets/` if the application is located in `./app` and
   * is launched from its parent directory. The assets path will be `./assets/`
   * if the application is launched from the same directory of the executable.
   *
   * @remark The assets path is appended with the base path and ends with a
   * forward slash.
   *
   * @sa abcg::Application::getBasePath
   */
  [[nodiscard]] static std::string const &getAssetsPath() {
    return m_assetsPath;
  }

  /**
   * @brief Returns the path to the application's directory, relative to the
   * directory the executable is launched in.
   *
   * @return Path to the directory that contains the application executable,
   * relative to the directory the executable is launched. For example, the base
   * path will be `./app` if the executable is located in `./app` and is called
   * from the parent directory. The base path will be `.` if the application is
   * launched from the same directory of the executable.
   *
   * @remark The returned path does not end with a slash.
   */
  [[nodiscard]] static std::string const &getBasePath() { return m_basePath; }

private:
  void mainLoopIterator(bool &done) const;

  Window *m_window{};

#if defined(__EMSCRIPTEN__)
  friend void mainLoopCallback(void *userData);
#endif

  // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
  // See https://bugs.llvm.org/show_bug.cgi?id=48040
  static inline std::string m_assetsPath{};
  static inline std::string m_basePath{};
  // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
};

#endif
