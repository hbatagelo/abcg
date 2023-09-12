/**
 * @file abcgWindow.hpp
 * @brief Header file of abcg::Window.
 *
 * Declaration of abcg::Window.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_WINDOW_HPP_
#define ABCG_WINDOW_HPP_

#include <string>

#include "abcgExternal.hpp"
#include "abcgTimer.hpp"

#if defined(__EMSCRIPTEN__)
#include "abcgOpenGLExternal.hpp"
#endif

namespace abcg {
struct WindowSettings;
class Application;
class Window;
int resizingEventWatcher(void *data, SDL_Event *event);
#if defined(__EMSCRIPTEN__)
EM_BOOL fullscreenchangeCallback(int eventType,
                                 EmscriptenFullscreenChangeEvent const *event,
                                 void *userData);
#endif
} // namespace abcg

/**
 * @brief Configuration settings of a window.
 *
 * This is a structure with configuration data related to the application's
 * window. These settings can be changed before and after window creation.
 *
 * @sa abcg::OpenGLWindow::setWindowSettings.
 * @sa abcg::OpenGLWindow::getWindowSettings.
 */
struct abcg::WindowSettings {
  /** @brief Window width in windowed mode, in pixels.
   *
   * @remark This is updated automatically when the window is resized, except
   * when it goes fullscreen. To query the actual window size, call
   * abcg::Window::getWindowSize.
   */
  int width{800};
  /** @brief Window height in windowed mode, in pixels.
   *
   * @remark This is updated automatically when the window is resized, except
   * when it goes fullscreen. To query the actual window size, call
   * abcg::Window::getWindowSize.
   */
  int height{600};
  /** @brief Whether to show an overlay window with a FPS counter. */
  bool showFPS{true};
  /** @brief Whether to show a button to toggle fullscreen on/off. */
  bool showFullscreenButton{true};
  /** @brief HTML element ID used for registering the fullscreen callback when
   * the application is built for WebAssembly.
   */
  std::string fullscreenElementID{"#canvas"};
  /** @brief String containing the window title. */
  std::string title{"ABCg Window"};
};

/**
 * @brief Base abstract class that represents a SDL window.
 *
 * This is the base class of the window classes customized for the different
 * graphics APIs, such as abcg::OpenGLWindow and abcg::VulkanWindow.
 *
 * @sa abcg::Window::handleEvent for custom handling of SDL events.
 * @sa abcg::Window::create for window initialization.
 * @sa abcg::Window::paint for frame rendering commands.
 * @sa abcg::Window::destroy for window termination.
 *
 * @remark Objects of this type cannot be copied or copy-constructed.
 */
class abcg::Window {
public:
  /**
   * @brief Default constructor.
   */
  Window() = default;
  Window(Window const &) = delete;
  /**
   * @brief Default move constructor.
   */
  Window(Window &&) = default;
  Window &operator=(Window const &) = delete;
  /**
   * @brief Default move assignment.
   */
  Window &operator=(Window &&) = default;
  /**
   * @brief Default destructor.
   */
  virtual ~Window() = default;

  [[nodiscard]] WindowSettings const &getWindowSettings() const noexcept;
  void setWindowSettings(WindowSettings const &windowSettings);

protected:
  /**
   * @brief Custom event handler.
   *
   * This is called when there is a pending event polled by SDL. abcg::Window
   * already handles resizing events, fullscreen toggling events, and ImGUI
   * events that are common to all windows.
   *
   * @param event SDL event.
   */
  virtual void handleEvent(SDL_Event const &event) = 0;

  /**
   * @brief Custom handler for window initialization tasks.
   *
   * This is called only once after SDL initialization.
   */
  virtual void create() = 0;

  /**
   * @brief Custom handler for window repainting.
   *
   * This is called continuously during the application's loop for redrawing the
   * window.
   */
  virtual void paint() = 0;

  /**
   * @brief Custom handler for window cleanup tasks.
   *
   * This is called only once when the SDL window is being terminated.
   */
  virtual void destroy() = 0;

  /**
   * @brief Returns the size of the SDL window.
   *
   * The window size in screen coordinates may differ from the size in pixels.
   * Override this function to take into account window size in pixels on
   * platforms with high-DPI support.
   *
   * @returns Size of the window (width, height), in screen coordinates.
   */
  [[nodiscard]] virtual glm::ivec2 getWindowSize() const = 0;

  [[nodiscard]] double getDeltaTime() const noexcept;
  [[nodiscard]] double getElapsedTime() const;
  [[nodiscard]] SDL_Window *getSDLWindow() const noexcept;
  [[nodiscard]] Uint32 getSDLWindowID() const noexcept;

  bool createSDLWindow(SDL_WindowFlags extraFlags);
  void setEnableResizingEventWatcher(bool enabled) noexcept;
  void toggleFullscreen();

private:
  void templateHandleEvent(SDL_Event const &event, bool &done);
  void templateCreate();
  void templatePaint();
  void templateDestroy();

  SDL_Window *m_window{};
  Uint32 m_windowID{};

  WindowSettings m_windowSettings;

  Timer m_deltaTime;
  Timer m_elapsedTime;
  double m_lastDeltaTime{};

  bool m_enableResizingEventWatcher{true};

  friend Application;
  friend int resizingEventWatcher(void *data, SDL_Event *event);
#if defined(__EMSCRIPTEN__)
  friend EM_BOOL
  fullscreenchangeCallback(int eventType,
                           EmscriptenFullscreenChangeEvent const *event,
                           void *userData);
#endif
};

#endif