/**
 * @file abcgWindow.cpp
 * @brief Definition of abcg::Window members.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgWindow.hpp"

#include <SDL_video.h>

#include <imgui_impl_sdl2.h>

namespace {
ImVec4 ColorAlpha(ImVec4 const &color, float const alpha) {
  return {color.x, color.y, color.z, alpha};
}

/**
 * @brief Sets up a custom color theme for ImGUI.
 *
 * @param darkTheme Whether to use a dark theme.
 * @param alpha Transparency factor (0=totally transparent, 1=totally opaque).
 */
void setupImGuiStyle(bool const darkTheme, float const alpha) {
  auto &style{ImGui::GetStyle()};

  ImVec4 const black{0.00f, 0.00f, 0.00f, 1.00f};
  ImVec4 const gray0{0.20f, 0.20f, 0.20f, 1.00f};
  ImVec4 const gray1{0.40f, 0.40f, 0.40f, 1.00f};
  ImVec4 const gray2{0.50f, 0.50f, 0.50f, 1.00f};
  ImVec4 const gray3{0.60f, 0.60f, 0.60f, 1.00f};
  ImVec4 const gray4{0.70f, 0.70f, 0.70f, 1.00f};
  ImVec4 const gray5{0.80f, 0.80f, 0.80f, 1.00f};
  ImVec4 const gray6{0.90f, 0.90f, 0.90f, 1.00f};
  ImVec4 const white{1.00f, 1.00f, 1.00f, 1.00f};

  // clang-format off
  style.Alpha                                 = 1.0f;
  style.FrameRounding                         = 4.0f;
  style.FrameBorderSize                       = 0.0f;
  style.WindowRounding                        = 5.0f;
  style.PopupRounding                         = 4.0f;
  style.GrabRounding                          = 3.0f;
  style.ChildRounding                         = 5.0f;
  style.ScrollbarSize                         = 15.0f;
  style.WindowTitleAlign                      = ImVec2(0.50f, 0.50f);
  style.Colors[ImGuiCol_Text]                 = ColorAlpha(black, 1.00f);
  style.Colors[ImGuiCol_TextDisabled]         = ColorAlpha(gray3, 1.00f);  
  style.Colors[ImGuiCol_WindowBg]             = ColorAlpha(gray5, 0.95f);
  style.Colors[ImGuiCol_ChildBg]              = ColorAlpha(white, 0.16f);
  style.Colors[ImGuiCol_PopupBg]              = ColorAlpha(gray6, 0.97f);
  style.Colors[ImGuiCol_Border]               = ColorAlpha(white, 0.20f);
  style.Colors[ImGuiCol_BorderShadow]         = ColorAlpha(white, 0.10f);
  style.Colors[ImGuiCol_FrameBg]              = ColorAlpha(gray6, 0.80f);
  style.Colors[ImGuiCol_FrameBgHovered]       = ColorAlpha(gray4, 1.00f);
  style.Colors[ImGuiCol_FrameBgActive]        = ColorAlpha(gray3, 0.67f);
  style.Colors[ImGuiCol_TitleBg]              = ColorAlpha(gray6, 0.95f);
  style.Colors[ImGuiCol_TitleBgCollapsed]     = ColorAlpha(gray4, 0.95f);
  style.Colors[ImGuiCol_TitleBgActive]        = ColorAlpha(gray4, 0.95f);
  style.Colors[ImGuiCol_MenuBarBg]            = ColorAlpha(gray6, 0.90f);
  style.Colors[ImGuiCol_ScrollbarBg]          = ColorAlpha(white, 0.00f);
  style.Colors[ImGuiCol_ScrollbarGrab]        = ColorAlpha(gray4, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ColorAlpha(gray3, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive]  = ColorAlpha(gray2, 1.00f);
  style.Colors[ImGuiCol_CheckMark]            = ColorAlpha(gray0, 1.00f);
  style.Colors[ImGuiCol_SliderGrab]           = ColorAlpha(gray1, 0.95f);
  style.Colors[ImGuiCol_SliderGrabActive]     = ColorAlpha(gray1, 1.00f);
  style.Colors[ImGuiCol_Button]               = ColorAlpha(gray1, 0.30f);
  style.Colors[ImGuiCol_ButtonHovered]        = ColorAlpha(gray2, 0.80f);
  style.Colors[ImGuiCol_ButtonActive]         = ColorAlpha(gray2, 0.90f);
  style.Colors[ImGuiCol_Header]               = ColorAlpha(gray4, 0.67f);
  style.Colors[ImGuiCol_HeaderHovered]        = ColorAlpha(gray3, 0.95f);
  style.Colors[ImGuiCol_HeaderActive]         = ColorAlpha(gray3, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip]           = ColorAlpha(white, 0.50f);
  style.Colors[ImGuiCol_ResizeGripHovered]    = ColorAlpha(gray2, 0.67f);
  style.Colors[ImGuiCol_ResizeGripActive]     = ColorAlpha(gray2, 0.95f);
  style.Colors[ImGuiCol_PlotLines]            = ColorAlpha(gray1, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered]     = ColorAlpha(gray0, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram]        = ColorAlpha(gray1, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ColorAlpha(gray0, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg]       = ColorAlpha(gray4, 0.67f);
  style.Colors[ImGuiCol_ModalWindowDimBg]     = ColorAlpha(gray0, 0.35f);
  style.Colors[ImGuiCol_NavHighlight]         = ColorAlpha(gray1, 1.00f);
  style.Colors[ImGuiCol_Tab]                  = ColorAlpha(gray1, 0.30f);
  style.Colors[ImGuiCol_TabHovered]           = ColorAlpha(gray2, 0.95f);
  style.Colors[ImGuiCol_TabActive]            = ColorAlpha(gray2, 0.90f);
  style.Colors[ImGuiCol_TabUnfocused]         = ColorAlpha(gray6, 0.80f);
  style.Colors[ImGuiCol_TabUnfocusedActive]   = ColorAlpha(gray5, 1.00f);
  style.Colors[ImGuiCol_DragDropTarget]       = ColorAlpha(black, 1.00f);
  style.Colors[ImGuiCol_Separator]            = ColorAlpha(gray2, 0.50f);
  style.Colors[ImGuiCol_SeparatorHovered]     = ColorAlpha(gray2, 0.67f);
  style.Colors[ImGuiCol_SeparatorActive]      = ColorAlpha(gray2, 0.95f);
  // clang-format on

  if (darkTheme) {
    for (ImVec4 &col : style.Colors) {
      auto hue{0.0f};
      auto sat{0.0f};
      auto val{0.0f};
      ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, hue, sat, val);

      if (sat < 0.1f) {
        val = 1.0f - val;
      }
      ImGui::ColorConvertHSVtoRGB(hue, sat, val, col.x, col.y, col.z);
      if (col.w < 1.00f) {
        col.w *= alpha;
      }
    }
  } else {
    for (ImVec4 &col : style.Colors) {
      if (col.w < 1.00f) {
        col.x *= alpha;
        col.y *= alpha;
        col.z *= alpha;
        col.w *= alpha;
      }
    }
  }
}
} // namespace

int abcg::resizingEventWatcher(void *data, SDL_Event *event) {
  if (event->type == SDL_WINDOWEVENT &&
      event->window.event == SDL_WINDOWEVENT_RESIZED) {
    auto *SDLWindow{SDL_GetWindowFromID(event->window.windowID)};
    if (SDLWindow == static_cast<SDL_Window *>(data)) {
      abcg::Window &window{*(
          static_cast<abcg::Window *>(SDL_GetWindowData(SDLWindow, "window")))};
      if (window.m_enableResizingEventWatcher) {
        [[maybe_unused]] bool done{};
        window.templateHandleEvent(*event, done);
        window.templatePaint();
      }
    }
  }
  return 0;
}

#if defined(__EMSCRIPTEN__)
EM_BOOL
abcg::fullscreenchangeCallback(int eventType,
                               EmscriptenFullscreenChangeEvent const *event,
                               void *userData) {
  auto &window{*(static_cast<abcg::Window *>(userData))};
  if (event->isFullscreen) {
    SDL_SetWindowSize(window.getSDLWindow(), event->screenWidth,
                      event->screenHeight);
  }
  return true;
}
#endif

/**
 * @brief Returns the time that have passed since the last frame.
 *
 * This delta time has a minimum resolution of 2ms. If the time is smaller than
 * that, zero is returned. Internally, the delta time accumulates for the next
 * frame(s) until at least 2ms have passed.
 *
 * @returns Time in seconds.
 */
double abcg::Window::getDeltaTime() const noexcept { return m_lastDeltaTime; }

/**
 * @brief Returns the time that have passed since the window was created.
 *
 * @returns Time in seconds.
 */
double abcg::Window::getElapsedTime() const { return m_elapsedTime.elapsed(); }

/**
 * @brief Returns the current configuration settings of the window.
 *
 * @returns Reference to the abcg::OpenGLWindow structure used by the window.
 */
abcg::WindowSettings const &abcg::Window::getWindowSettings() const noexcept {
  return m_windowSettings;
}

/**
 * @brief Sets the configuration settings of the window.
 */
void abcg::Window::setWindowSettings(WindowSettings const &windowSettings) {
  if (m_window != nullptr) {
    if (windowSettings.title != m_windowSettings.title) {
      SDL_SetWindowTitle(m_window, windowSettings.title.c_str());
    }

    if (windowSettings.width != m_windowSettings.width ||
        windowSettings.height != m_windowSettings.height) {
      SDL_SetWindowSize(m_window, windowSettings.width, windowSettings.height);
    }

#if defined(__EMSCRIPTEN__)
    if (windowSettings.fullscreenElementID !=
        m_windowSettings.fullscreenElementID) {
      emscripten_set_fullscreenchange_callback(
          windowSettings.fullscreenElementID.c_str(), this, true,
          fullscreenchangeCallback);
    }
#endif
  }

  m_windowSettings = windowSettings;
}

/**
 * @brief Returns the SDL window previously created with
 * abcg::Window::createOpenGLWindow or abcg::Window::createVulkanWindow.
 *
 * @returns Pointer to the SDL window.
 *
 * @remark The pointer is nullptr if the window was not created.
 */
SDL_Window *abcg::Window::getSDLWindow() const noexcept { return m_window; }

/**
 * @brief Returns the numeric ID of the SDL window previously created with
 * abcg::Window::createOpenGLWindow or abcg::Window::createVulkanWindow.
 *
 * @returns ID of the window.
 *
 * @remark The ID is 0 if the window was not created.
 */
Uint32 abcg::Window::getSDLWindowID() const noexcept { return m_windowID; }

/**
 * @brief Creates the SDL window.
 *
 * @param extraFlags Extra SDL window flags to be combined with the common
 * flags (`SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI`).
 *
 * @returns `true` on success; `false` on failure.
 */
bool abcg::Window::createSDLWindow(SDL_WindowFlags extraFlags) {
  if (m_window != nullptr)
    return false;

  auto commonFlags{SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI};

  m_window = SDL_CreateWindow(
      m_windowSettings.title.c_str(), SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, m_windowSettings.width, m_windowSettings.height,
      gsl::narrow<Uint32>(extraFlags) | gsl::narrow<Uint32>(commonFlags));
  if (m_window == nullptr)
    return false;

#if defined(WIN32)
  SDL_SetWindowData(m_window, "window", this);
  SDL_AddEventWatch(resizingEventWatcher, m_window);
#endif

#if defined(__EMSCRIPTEN__)
  emscripten_set_fullscreenchange_callback(
      getWindowSettings().fullscreenElementID.c_str(), this, true,
      fullscreenchangeCallback);
#endif

  m_windowID = SDL_GetWindowID(m_window);
  return true;
}

/**
 * @brief Toggles the resizing event watcher on/off.
 *
 * The resizing event watcher enables rendering while resizing in windowed mode.
 * This is only required on Windows.
 *
 * @param enabled Whether to enable the resizing event watcher.
 */
void abcg::Window::setEnableResizingEventWatcher(bool enabled) noexcept {
  m_enableResizingEventWatcher = enabled;
}

/**
 * @brief Toggles between fullscreen and windowed mode.
 */
void abcg::Window::toggleFullscreen() {
#if defined(__EMSCRIPTEN__)
  EM_ASM(toggleFullscreen(););
#else
  // Temporarily disable the resizing event watcher to avoid rendering a new
  // frame while the current one has not finished.
  setEnableResizingEventWatcher(false);

  constexpr Uint32 windowFlags{SDL_WINDOW_FULLSCREEN |
                               SDL_WINDOW_FULLSCREEN_DESKTOP};
  bool const fullscreen{(SDL_GetWindowFlags(m_window) & windowFlags) != 0U};
  enum class WindowType { Windowed, Fullscreen /*, FullscreenExclusive*/ };

  switch (auto const desiredWindowType{fullscreen ? WindowType::Windowed
                                                  : WindowType::Fullscreen};
          desiredWindowType) {
  case WindowType::Windowed:
    SDL_SetWindowFullscreen(getSDLWindow(), 0);
    SDL_SetWindowSize(m_window, m_windowSettings.width,
                      m_windowSettings.height);
    break;
    //  case WindowType::FullscreenExclusive:
    //    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
    //    break;
  case WindowType::Fullscreen:
    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    break;
  }
  SDL_ShowCursor(static_cast<int>(fullscreen));

  setEnableResizingEventWatcher(true);
#endif
}

void abcg::Window::templateHandleEvent(SDL_Event const &event, bool &done) {
  ImGui_ImplSDL2_ProcessEvent(&event);

  if (event.window.windowID != m_windowID)
    return;

  if (event.type == SDL_WINDOWEVENT) {
    switch (event.window.event) {
    case SDL_WINDOWEVENT_CLOSE:
      done = true;
      break;

    case SDL_WINDOWEVENT_RESIZED: {
      auto const fullscreen{
          (SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN) != 0U};
      if (!fullscreen) {
        m_windowSettings.width = event.window.data1;
        m_windowSettings.height = event.window.data2;
      }
#if defined(__EMSCRIPTEN__)
      m_windowSettings.width = event.window.data1;
      m_windowSettings.height = event.window.data2;
      SDL_SetWindowSize(m_window, m_windowSettings.width,
                        m_windowSettings.height);
#endif
    } break;
    default:
      break;
    }
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_F11) {
#if defined(__EMSCRIPTEN__)
      auto const isFullscreenAvailable{
          gsl::narrow<bool>(
              EM_ASM_INT({ return document.fullscreenEnabled; })) &&
          gsl::narrow<bool>(EM_ASM_INT({ return !isMobile(); }))};
      if (isFullscreenAvailable)
#endif
        toggleFullscreen();
    }
  }

  // Won't pass mouse events to the application if ImGUI has captured the
  // mouse
  auto useCustomEventHandler{true};
  if (ImGui::GetIO().WantCaptureMouse &&
      (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN ||
       event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL)) {
    useCustomEventHandler = false;
  }

  // Won't pass keyboard events to the application if ImGUI has captured the
  // keyboard
  if (ImGui::GetIO().WantCaptureKeyboard &&
      (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP ||
       event.type == SDL_TEXTEDITING || event.type == SDL_TEXTINPUT ||
       event.type == SDL_KEYMAPCHANGED)) {
    useCustomEventHandler = false;
  }

  if (useCustomEventHandler) {
    handleEvent(event);
  }
}

void abcg::Window::templateCreate() {
  m_deltaTime.restart();
  m_elapsedTime.restart();

  create();

  // Set up our own Dear ImGui style
  setupImGuiStyle(true, 1.0f);
}

void abcg::Window::templatePaint() {
  // Cap to 480 Hz
  if (m_deltaTime.elapsed() >= 1.0 / 480.0) {
    m_lastDeltaTime = m_deltaTime.restart();
  } else {
    m_lastDeltaTime = 0.0;
  }

  paint();
}

void abcg::Window::templateDestroy() {
  if (m_window == nullptr)
    return;

  destroy();

  SDL_DestroyWindow(m_window);
  m_window = nullptr;
  m_windowID = 0;
}