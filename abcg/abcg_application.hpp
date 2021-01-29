/**
 * @file abcg_application.hpp
 * @brief abcg::Application header file.
 *
 * Declaration of abcg::Application class.
 *
 * This project is released under the MIT License.
 */

#ifndef ABCG_APPLICATION_HPP_
#define ABCG_APPLICATION_HPP_

#include <memory>
#include <string>
#include <vector>

#include "abcg_exception.hpp"
#include "abcg_openglwindow.hpp"

namespace abcg {
class Application;
class OpenGLWindow;
#if defined(__EMSCRIPTEN__)
void mainLoopCallback(void* userData);
#endif
}  // namespace abcg

/**
 * @brief abcg::Application class.
 *
 * This is the main application class that starts an ABCg application.
 *
 */
class abcg::Application {
 public:
  Application(int argc, char** argv);
  virtual ~Application();

  Application(const Application&) = delete;
  Application(Application&&) = default;
  Application& operator=(const Application&) = delete;
  Application& operator=(Application&&) = default;

  template <typename T>
  // requires std::derived_from<T, abcg::OpenGLWindow>
  void run(std::unique_ptr<T>& window);
  void run(std::vector<std::unique_ptr<OpenGLWindow>>& windows);

 private:
  void mainLoopIterator(bool& done);
  void run();

  std::string m_basePath;
  std::vector<std::unique_ptr<OpenGLWindow>> m_windows;

#if defined(__EMSCRIPTEN__)
  friend void mainLoopCallback(void* userData);
#endif
};

/**
 * @brief Runs the application for a single window.
 *
 * @param window Pointer to a window.
 *
 * @throw abcg::Exception if window is a null pointer or cannot be cast to a
 * pointer to abcg::OpenGLWindow.
 */
template <typename T>
// requires std::derived_from<T, abcg::OpenGLWindow>
void abcg::Application::run([[maybe_unused]] std::unique_ptr<T>& window) {
  if (auto* pointerToBase{dynamic_cast<abcg::OpenGLWindow*>(window.get())}) {
    window.release();
    auto uniquePointerToBase{
        std::unique_ptr<abcg::OpenGLWindow>(pointerToBase)};
    m_windows.push_back(std::move(uniquePointerToBase));
    run();
  } else {
    throw abcg::Exception{abcg::Exception::Runtime("Invalid pointer")};
  }
}

#endif