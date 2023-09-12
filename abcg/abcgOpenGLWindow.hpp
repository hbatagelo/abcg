/**
 * @file abcgOpenGLWindow.hpp
 * @brief Header file of abcg::OpenGLWindow.
 *
 * Declaration of abcg::OpenGLWindow.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_OPENGL_WINDOW_HPP_
#define ABCG_OPENGL_WINDOW_HPP_

#include <string>

#include "abcgExternal.hpp"
#include "abcgOpenGLFunction.hpp"
#include "abcgWindow.hpp"

namespace abcg {
enum class OpenGLProfile;
class OpenGLWindow;
struct OpenGLSettings;
} // namespace abcg

/**
 * @brief Enumeration of OpenGL profiles.
 *
 * @sa abcg::OpenGLSettings.
 */
enum class abcg::OpenGLProfile {
  /** @brief OpenGL core profile.
   *
   * Deprecated functions are disabled.
   */
  Core,
  /** @brief OpenGL compatibility profile.
   *
   * Deprecated functions are allowed.
   */
  Compatibility,
  /** @brief OpenGL ES profile.
   *
   * Only a subset of the base OpenGL functionality is available.
   */
  ES
};

/**
 * @brief Configuration settings for creating an OpenGL context.
 *
 * This structure contains the configuration settings to be used when creating
 * the OpenGL context. These must be set before calling
 * `abcg::Application::run`.
 *
 * @sa abcg::OpenGLWindow::getOpenGLSettings.
 * @sa abcg::OpenGLWindow::setOpenGLSettings.
 */
struct abcg::OpenGLSettings {
  /** @brief Type of OpenGL context. */
  OpenGLProfile profile{OpenGLProfile::Core};
  /** @brief OpenGL context major version. */
  int majorVersion{3};
  /** @brief OpenGL context minor version. */
  int minorVersion{3};
  /** @brief Minimum number of bits in the depth buffer. */
  int depthBufferSize{24};
  /** @brief Minimum number of bits in the stencil buffer. */
  int stencilBufferSize{0};
  /** @brief Number of samples used around the current pixel used for
   * multisample anti-aliasing. */
  int samples{0};
  /** @brief Whether the swapping of the front and back frame buffers is
   * synchronized with the vertical retrace. */
  bool vSync{false};
  /** @brief Whether the output is double buffered. */
  bool doubleBuffering{true};
};

/**
 * @brief Base class for a window that displays graphics using an OpenGL
 * context.
 *
 * Derive from this class to create a custom OpenGL window object to be passed
 * to abcg::Application::run.
 *
 * @sa abcg::OpenGLWindow::onEvent for handling SDL events.
 * @sa abcg::OpenGLWindow::onCreate for initializing OpenGL resources.
 * @sa abcg::OpenGLWindow::onPaint for scene rendering.
 * @sa abcg::OpenGLWindow::onPaintUI for UI rendering.
 * @sa abcg::OpenGLWindow::onResize for handling of window resize events.
 * @sa abcg::OpenGLWindow::onUpdate for commands to be called every frame.
 * @sa abcg::OpenGLWindow::onDestroy for cleaning up OpenGL resources.

 * @remark Objects of this type cannot be copied or copy-constructed.
 */
class abcg::OpenGLWindow : public Window {
public:
  [[nodiscard]] OpenGLSettings const &getOpenGLSettings() const noexcept;
  void setOpenGLSettings(OpenGLSettings const &openGLSettings) noexcept;
  void saveScreenshotPNG(std::string_view filename) const;

protected:
  virtual void onEvent(SDL_Event const &event);
  virtual void onCreate();
  virtual void onPaint();
  virtual void onPaintUI();
  virtual void onResize(glm::ivec2 const &size);
  virtual void onUpdate();
  virtual void onDestroy();

private:
  void handleEvent(SDL_Event const &event) final;
  void create() final;
  void paint() final;
  void destroy() final;
  [[nodiscard]] glm::ivec2 getWindowSize() const final;

  OpenGLSettings m_openGLSettings;
  std::string m_GLSLVersion;
  SDL_GLContext m_GLContext{};
  bool m_hidden{};
  bool m_minimized{};
};

#endif
