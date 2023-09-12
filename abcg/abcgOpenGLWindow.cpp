/**
 * @file abcgOpenGLWindow.cpp
 * @brief Definition of abcg::OpenGLWindow members.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgOpenGLWindow.hpp"

#include <SDL_events.h>
#include <SDL_image.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include "abcgEmbeddedFonts.hpp"
#include "abcgException.hpp"
#include "abcgWindow.hpp"

/**
 * @brief Returns the configuration settings of the OpenGL context.
 *
 * @returns Reference to the abcg::OpenGLSettings structure used for creating
 * the OpenGL context.
 */
abcg::OpenGLSettings const &
abcg::OpenGLWindow::getOpenGLSettings() const noexcept {
  return m_openGLSettings;
}

/**
 * @brief Sets the configuration settings that will be used for creating the
 * OpenGL context.
 *
 * This function will have no effect if called after the creation of the
 * OpenGL context.
 */
void abcg::OpenGLWindow::setOpenGLSettings(
    OpenGLSettings const &openGLSettings) noexcept {
  if (abcg::Window::getSDLWindow() != nullptr)
    return;
  m_openGLSettings = openGLSettings;
}

/**
 * @brief Takes a snapshot of the screen and saves it to a file.
 *
 * @param filename String view to the filename.
 */
void abcg::OpenGLWindow::saveScreenshotPNG(std::string_view filename) const {
  auto const size{getWindowSize()};
  auto const bitsPerPixel{8};
  auto const channels{4};
  auto const pitch{gsl::narrow<long>(size.x * channels)};

  auto const numPixels{gsl::narrow<std::size_t>(size.x * size.y * channels)};
  std::vector<unsigned char> pixels(numPixels);
  glReadBuffer(m_openGLSettings.doubleBuffering ? GL_BACK : GL_FRONT);
  glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

  // Flip upside down
  for (auto const line : iter::range(size.y / 2)) {
    std::swap_ranges(pixels.begin() + pitch * line,
                     pixels.begin() + pitch * (line + 1),
                     pixels.begin() + pitch * (size.y - line - 1));
  }

  if (auto *const surface{SDL_CreateRGBSurfaceFrom(
          pixels.data(), size.x, size.y, channels * bitsPerPixel,
          gsl::narrow<int>(pitch), 0x000000FF, 0x0000FF00, 0x00FF0000,
          0xFF000000)}) {
    IMG_SavePNG(surface, filename.data());
    SDL_FreeSurface(surface);
  }
}

/**
 * @brief Custom event handler.
 *
 * This virtual function is called whenever there is a pending event polled by
 * SDL.
 *
 * Override it for custom behavior. By default, it does nothing.
 *
 * @param event Event structure containing information about the pending event.
 */
void abcg::OpenGLWindow::onEvent([[maybe_unused]] SDL_Event const &event) {}

/**
 * @brief Custom handler for OpenGL initialization tasks to be performed before
 * rendering the scene.
 *
 * This virtual function is called once after the OpenGL context is created.
 *
 * Override it for custom behavior. By default, it only calls `glClearColor`
 * with the RGBA color (0, 0, 0, 1).
 */
void abcg::OpenGLWindow::onCreate() { glClearColor(0, 0, 0, 1); }

/**
 * @brief Custom handler for rendering the OpenGL scene.
 *
 * This virtual function is called for each frame of the rendering loop, just
 * after abcg::OpenGLWindow::onPaintUI.
 *
 * This is not called when the window is minimized.
 *
 * Override it for custom behavior. By default, it clears the color buffer and
 * calls `glViewport(0, 0, w, h)`, where `w` is the width, and `h` is the height
 * of the window.
 */
void abcg::OpenGLWindow::onPaint() {
  glClear(GL_COLOR_BUFFER_BIT);
  auto const size{getWindowSize()};
  glViewport(0, 0, size.x, size.y);
}

/**
 * @brief Custom handler for rendering Dear ImGUI controls.
 *
 * This virtual function is called for each frame of the rendering loop, just
 * before abcg::OpenGLWindow::onPaint.
 *
 * This is not called when the window is minimized.
 *
 * Override it for custom behavior. By default, it shows a FPS counter if
 * abcg::WindowSettings::showFPS is set to `true`, and a toggle fullscreen
 * button if abcg::WindowSettings::showFullscreenButton is set to `true`.
 */
void abcg::OpenGLWindow::onPaintUI() {
  // FPS counter
  if (abcg::Window::getWindowSettings().showFPS) {
    auto fps{ImGui::GetIO().Framerate};

    static auto offset{0UL};
    static auto refreshTime{ImGui::GetTime()};
    static std::array<float, 150> frames{};

    while (refreshTime < ImGui::GetTime()) {
      auto const refreshFrequency{60.0};
      frames.at(offset) = fps;
      offset = (offset + 1) % frames.size();
      refreshTime += (1.0 / refreshFrequency);
    }

    ImGui::SetNextWindowPos(ImVec2(5, 5));
    ImGui::Begin("FPS", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoFocusOnAppearing);
    auto const label{fmt::format("avg {:.1f} FPS", fps)};
    ImGui::PlotLines("", frames.data(), gsl::narrow<int>(frames.size()),
                     gsl::narrow<int>(offset), label.c_str(), 0.0f,
                     // *std::ranges::max_element(frames) * 2,
                     *std::max_element(frames.begin(), frames.end()) * 2,
                     ImVec2(gsl::narrow<float>(frames.size()), 50));
    ImGui::End();
  }

  // Fullscreen button
  if (abcg::Window::getWindowSettings().showFullscreenButton) {
#if defined(__EMSCRIPTEN__)
    auto const isFullscreenAvailable{
        gsl::narrow<bool>(EM_ASM_INT({ return document.fullscreenEnabled; })) &&
        gsl::narrow<bool>(EM_ASM_INT({ return !isMobile(); }))};
    if (isFullscreenAvailable)
#endif
    {
      auto const windowSize{getWindowSize()};
      auto const widgetSize{ImVec2(150, 30)};
      auto const windowBorder{ImVec2(16, 16)};

      ImGui::SetNextWindowSize(
          ImVec2(widgetSize.x + windowBorder.x, widgetSize.y + windowBorder.y));
      ImGui::SetNextWindowPos(ImVec2(5, gsl::narrow<float>(windowSize.y) -
                                            (widgetSize.y + windowBorder.y) -
                                            5));

      auto const windowFlags{ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoBringToFrontOnFocus |
                             ImGuiWindowFlags_NoFocusOnAppearing};
      ImGui::Begin("Fullscreen", nullptr, windowFlags);

      if (ImGui::Button("Toggle fullscreen", widgetSize)) {
        abcg::Window::toggleFullscreen();
      }

      ImGui::End();
    }
  }
}

/**
 * @brief Custom handler for window resizing events.
 *
 * This is a hook function called whenever the window is resized.
 *
 * @param size New window size (width, height), in pixels.
 *
 * Override it for custom behavior. By default, it does nothing.
 */
void abcg::OpenGLWindow::onResize([[maybe_unused]] glm::ivec2 const &size) {}

/**
 * @brief Custom handler called for each frame before painting.
 *
 * This virtual function is called just before abcg::VulkanWindow::onPaint, even
 * if the window is minimized.
 *
 * Override it for custom behavior. By default, it does nothing.
 */
void abcg::OpenGLWindow::onUpdate() {}

/**
 * @brief Custom handler for cleaning up OpenGL resources.
 *
 * This is a hook function called when the application is exiting.
 *
 * Override it for custom behavior. By default, it does nothing.
 */
void abcg::OpenGLWindow::onDestroy() {}

void abcg::OpenGLWindow::handleEvent(SDL_Event const &event) {
  if (event.window.windowID != abcg::Window::getSDLWindowID())
    return;

  if (event.type == SDL_WINDOWEVENT) {
    switch (event.window.event) {
    case SDL_WINDOWEVENT_HIDDEN:
      m_hidden = true;
      break;
    case SDL_WINDOWEVENT_SHOWN:
    case SDL_WINDOWEVENT_EXPOSED:
      m_hidden = false;
      break;
    case SDL_WINDOWEVENT_MINIMIZED:
      m_minimized = true;
      break;
    case SDL_WINDOWEVENT_RESTORED:
      m_minimized = false;
      break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
    case SDL_WINDOWEVENT_RESIZED: {
      onResize(getWindowSize());
    } break;
    default:
      break;
    }
  }

  onEvent(event);
}

void abcg::OpenGLWindow::create() {
#if defined(__EMSCRIPTEN__)
  if (!m_openGLSettings.doubleBuffering) {
    emscripten_run_script(
        "canvas.getContext('webgl2', {preserveDrawingBuffer : true});\n"
        "canvas.style.backgroundColor = '#000000';");
  }
#endif

  // Shortcuts
  auto &majorVersion{m_openGLSettings.majorVersion};
  auto &minorVersion{m_openGLSettings.minorVersion};
  auto &profile{m_openGLSettings.profile};

#if defined(__EMSCRIPTEN__)
  profile = OpenGLProfile::ES; // WebGL 2.0
#elif defined(__APPLE__)
  profile = OpenGLProfile::Core;
  majorVersion = std::min(majorVersion, 4);
  if (majorVersion == 4)
    minorVersion = 1;
#endif

  if (profile == OpenGLProfile::ES) {
    majorVersion = 3;
#if defined(__EMSCRIPTEN__)
    minorVersion = 0;
#else
    minorVersion = std::min(std::max(minorVersion, 0), 2);
#endif
  } else {
    // Constrain to the range [3.3, 4.0, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6]
    majorVersion = std::min(std::max(majorVersion, 3), 4);
    if (majorVersion == 3) {
      minorVersion = 3;
    } else {
      minorVersion = std::min(std::max(minorVersion, 0), 6);
    }
  }

  m_GLSLVersion =
      fmt::format("#version {:d}{:02d}", majorVersion, minorVersion * 10);

  switch (profile) {
  case OpenGLProfile::Core:
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    m_GLSLVersion += " core";
    break;
  case OpenGLProfile::Compatibility:
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    m_GLSLVersion += " compatibility";
    break;
  case OpenGLProfile::ES:
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    m_GLSLVersion += " es";
    break;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,
                      m_openGLSettings.doubleBuffering ? 1 : 0);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_openGLSettings.depthBufferSize);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_openGLSettings.stencilBufferSize);

  if (m_openGLSettings.samples > 0) {
    // Enable multisampling
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // Can be 2, 4, 8 or 16
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_openGLSettings.samples);
  } else {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
  }

  // Create window with graphics context
  while (true) {
    if (!createSDLWindow(SDL_WINDOW_OPENGL) && m_openGLSettings.samples > 0) {
      // Try again, but this time with multisampling disabled
      m_openGLSettings.samples = 0;
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
      fmt::print("Warning: multisampling requested but not supported!\n");
    } else {
      break;
    }
  }

  if (abcg::Window::getSDLWindow() == nullptr) {
    throw abcg::SDLError("SDL_CreateWindow failed");
  }

  // Create OpenGL context
  m_GLContext = SDL_GL_CreateContext(abcg::Window::getSDLWindow());
  if (m_GLContext == nullptr) {
    throw abcg::SDLError("SDL_GL_CreateContext failed");
  }

#if !defined(__EMSCRIPTEN__)
  SDL_GL_SetSwapInterval(m_openGLSettings.vSync ? 1 : 0);
#endif

#if !defined(__EMSCRIPTEN__)
  if (auto const err{glewInit()}; GLEW_OK != err) {
    throw abcg::Exception{
        fmt::format("Failed to initialize OpenGL loader: {}",
                    reinterpret_cast<char const *>(glewGetErrorString(err)))};
  }
  fmt::print("Using GLEW.....: {}\n",
             reinterpret_cast<char const *>(glewGetString(GLEW_VERSION)));
#endif

  fmt::print("OpenGL vendor..: {}\n",
             reinterpret_cast<char const *>(glGetString(GL_VENDOR)));
  fmt::print("OpenGL renderer: {}\n",
             reinterpret_cast<char const *>(glGetString(GL_RENDERER)));
  fmt::print("OpenGL version.: {}\n",
             reinterpret_cast<char const *>(glGetString(GL_VERSION)));
  fmt::print(
      "GLSL version...: {}\n",
      reinterpret_cast<char const *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

  // Print out extensions
  // GLint numExtensions{};
  // glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
  // for (auto const index : iter::range(gsl::narrow<GLuint>(numExtensions))) {
  //   fmt::print(
  //       "GL extension {}: {}\n", index,
  //       reinterpret_cast<char const *>(glGetStringi(GL_EXTENSIONS, index)));
  // }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &guiIO{ImGui::GetIO()};
  // Enable keyboard controls
  guiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // Enable gamepad controls
  guiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  // For an Emscripten build we are disabling file-system access, so let's
  // not attempt to do a fopen() of the imgui.ini file. You may manually
  // call LoadIniSettingsFromMemory() to load settings from your own storage.
  guiIO.IniFilename = nullptr;

  // Setup platform/renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(abcg::Window::getSDLWindow(), m_GLContext);
  ImGui_ImplOpenGL3_Init(m_GLSLVersion.c_str());

  // Load fonts
  guiIO.Fonts->Clear();

  ImFontConfig fontConfig;
  fontConfig.FontDataOwnedByAtlas = false;
  if (std::array ttf{INCONSOLATA_MEDIUM_TTF};
      guiIO.Fonts->AddFontFromMemoryTTF(ttf.data(),
                                        gsl::narrow<int>(ttf.size()), 16.0f,
                                        &fontConfig) == nullptr) {
    throw abcg::RuntimeError("Failed to load font file");
  }

  onCreate();

  onResize(getWindowSize());
}

void abcg::OpenGLWindow::paint() {
  onUpdate();

  if (m_hidden || m_minimized)
    return;

  SDL_GL_MakeCurrent(abcg::Window::getSDLWindow(), m_GLContext);

#if defined(__EMSCRIPTEN__)
  // Force window size in windowed mode
  EmscriptenFullscreenChangeEvent fullscreenStatus{};
  emscripten_get_fullscreen_status(&fullscreenStatus);
  if (fullscreenStatus.isFullscreen == EM_FALSE) {
    auto windowSettings{abcg::Window::getWindowSettings()};
    SDL_SetWindowSize(abcg::Window::getSDLWindow(), windowSettings.width,
                      windowSettings.height);
  }
#endif

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  onPaintUI();

  ImGui::Render();

  onPaint();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  if (m_openGLSettings.doubleBuffering) {
    SDL_GL_SwapWindow(abcg::Window::getSDLWindow());
  } else {
    glFinish();
  }
}

void abcg::OpenGLWindow::destroy() {
  onDestroy();

  if (ImGui::GetCurrentContext() != nullptr) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
  }
  if (m_GLContext != nullptr) {
    SDL_GL_DeleteContext(m_GLContext);
    m_GLContext = nullptr;
  }
}

[[nodiscard]] glm::ivec2 abcg::OpenGLWindow::getWindowSize() const {
  glm::ivec2 size{};
  if (auto *window{abcg::Window::getSDLWindow()}; window != nullptr) {
    SDL_GL_GetDrawableSize(window, &size.x, &size.y);
  }
  return size;
}
