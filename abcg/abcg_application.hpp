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

#include "abcg_exception.hpp"

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

  void run(std::unique_ptr<OpenGLWindow> window);

 private:
  void mainLoopIterator(bool& done);
  void run();

  std::string m_basePath;
  std::unique_ptr<OpenGLWindow> m_window;

#if defined(__EMSCRIPTEN__)
  friend void mainLoopCallback(void* userData);
#endif
};

#endif