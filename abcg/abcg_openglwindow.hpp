/**
 * @file abcg_openglwindow.hpp
 * @brief abcg::OpenGLWindow header file.
 *
 * Declaration of abcg::OpenGLWindow.
 *
 * This project is released under the MIT License.
 */

#ifndef ABCG_OPENGLWINDOW_HPP_
#define ABCG_OPENGLWINDOW_HPP_

#include <string>

#include "abcg_elapsedtimer.hpp"
#include "abcg_openglfunctions.hpp"

namespace abcg {
enum class OpenGLProfile;
class Application;
class OpenGLWindow;
struct OpenGLSettings;
struct WindowSettings;
#if defined(__EMSCRIPTEN__)
EM_BOOL fullscreenchangeCallback(int eventType,
                                 const EmscriptenFullscreenChangeEvent* event,
                                 void* userData);
#endif
}  // namespace abcg

/**
 * @brief Enumeration of OpenGL profiles.
 *
 */
enum class abcg::OpenGLProfile { Core, Compatibility, ES };

struct alignas(32) abcg::OpenGLSettings {
  OpenGLProfile profile{OpenGLProfile::Core};
  int majorVersion{4};
  int minorVersion{1};
  int depthBufferSize{24};
  int stencilSize{8};
  int samples{0};
  bool vsync{false};
  bool preserveWebGLDrawingBuffer{false};
};

struct alignas(64) abcg::WindowSettings {
  int width{800};
  int height{600};
  bool showFPS{true};
  bool showFullscreenButton{true};
  std::string title{"ABCg Window"};
};

/**
 * @brief abcg::OpenGLWindow class.
 *
 */
class abcg::OpenGLWindow {
 public:
  OpenGLWindow() = default;
  virtual ~OpenGLWindow();

  OpenGLWindow(const OpenGLWindow&) = default;
  OpenGLWindow(OpenGLWindow&&) = default;
  OpenGLWindow& operator=(const OpenGLWindow&) = default;
  OpenGLWindow& operator=(OpenGLWindow&&) = default;

  [[nodiscard]] OpenGLSettings getOpenGLSettings() noexcept;
  [[nodiscard]] WindowSettings getWindowSettings() noexcept;
  void setOpenGLSettings(const OpenGLSettings& openGLSettings) noexcept;
  void setWindowSettings(const WindowSettings& windowSettings);

 protected:
  virtual void handleEvent(SDL_Event& event);
  virtual void initializeGL();
  virtual void paintGL();
  virtual void paintUI();
  virtual void resizeGL(int width, int height);
  virtual void terminateGL();

  [[nodiscard]] GLuint createProgramFromFile(
      std::string_view pathToVertexShader,
      std::string_view pathToFragmentShader);
  [[nodiscard]] GLuint createProgramFromString(
      std::string_view vertexShaderSource,
      std::string_view fragmentShaderSource);
  std::string getAssetsPath();
  [[nodiscard]] double getDeltaTime() const;
  [[nodiscard]] double getElapsedTime() const;
  void toggleFullscreen();

 private:
  void handleEvent(SDL_Event& event, bool& done);
  void initialize(std::string_view basePath);
  void paint();

  WindowSettings m_windowSettings{};
  OpenGLSettings m_openGLSettings{};

  std::string m_assetsPath{};
  std::string m_GLSLVersion{};

  SDL_Window* m_window{};
  SDL_GLContext m_GLContext{};
  Uint32 m_windowID{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  ElapsedTimer m_deltaTime;
  ElapsedTimer m_windowStartTime;
  double m_lastDeltaTime{0.0};

  friend Application;

#if defined(__EMSCRIPTEN__)
  friend EM_BOOL fullscreenchangeCallback(
      int eventType, const EmscriptenFullscreenChangeEvent* event,
      void* userData);
#endif
};

#endif