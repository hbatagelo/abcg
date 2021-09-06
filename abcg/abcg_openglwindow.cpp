/**
 * @file abcg_openglwindow.cpp
 * @brief Definition of abcg::OpenGLWindow class.
 *
 * This project is released under the MIT License.
 */

#include "abcg_openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <string_view>

#include "SDL_events.h"
#include "SDL_video.h"
#include "abcg_application.hpp"
#include "abcg_embeddedfonts.hpp"
#include "abcg_string.hpp"

void printShaderInfoLog(GLuint shader, std::string_view prefix) {
  GLint infoLogLength{};
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (infoLogLength > 0) {
    std::vector<GLchar> infoLog{};
    infoLog.reserve(static_cast<size_t>(infoLogLength));
    glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());
    fmt::print("{} information log:\n{}\n", prefix, infoLog.data());
  }
}

void printProgramInfoLog(GLuint program) {
  GLint infoLogLength{};
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (infoLogLength > 0) {
    std::vector<GLchar> infoLog{};
    infoLog.reserve(static_cast<size_t>(infoLogLength));
    glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());
    fmt::print("Program information log:\n{}\n", infoLog.data());
  }
}

ImVec4 ColorAlpha(const ImVec4 &color, float alpha) {
  return ImVec4(color.x, color.y, color.z, alpha);
}

void setupImGuiStyle(bool darkTheme, float alpha) {
  auto &style = ImGui::GetStyle();

  ImVec4 black{0.00f, 0.00f, 0.00f, 1.00f};
  ImVec4 gray0{0.20f, 0.20f, 0.20f, 1.00f};
  ImVec4 gray1{0.40f, 0.40f, 0.40f, 1.00f};
  ImVec4 gray2{0.50f, 0.50f, 0.50f, 1.00f};
  ImVec4 gray3{0.60f, 0.60f, 0.60f, 1.00f};
  ImVec4 gray4{0.70f, 0.70f, 0.70f, 1.00f};
  ImVec4 gray5{0.80f, 0.80f, 0.80f, 1.00f};
  ImVec4 gray6{0.90f, 0.90f, 0.90f, 1.00f};
  ImVec4 white{1.00f, 1.00f, 1.00f, 1.00f};

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
  style.Colors[ImGuiCol_Header]               = ColorAlpha(gray4, 0.50f);
  style.Colors[ImGuiCol_HeaderHovered]        = ColorAlpha(gray3, 0.80f);
  style.Colors[ImGuiCol_HeaderActive]         = ColorAlpha(gray3, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip]           = ColorAlpha(white, 0.50f);
  style.Colors[ImGuiCol_ResizeGripHovered]    = ColorAlpha(gray2, 0.67f);
  style.Colors[ImGuiCol_ResizeGripActive]     = ColorAlpha(gray2, 0.95f);
  style.Colors[ImGuiCol_PlotLines]            = ColorAlpha(gray1, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered]     = ColorAlpha(gray0, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram]        = ColorAlpha(gray1, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ColorAlpha(gray0, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg]       = ColorAlpha(gray6, 0.35f);
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
      float H{};
      float S{};
      float V{};
      ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

      if (S < 0.1f) {
        V = 1.0f - V;
      }
      ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
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

#if defined(__EMSCRIPTEN__)
EM_BOOL abcg::fullscreenchangeCallback(
    int eventType, const EmscriptenFullscreenChangeEvent *event,
    void *userData) {
  abcg::OpenGLWindow &window = *(static_cast<abcg::OpenGLWindow *>(userData));
  if (event->isFullscreen) {
    SDL_SetWindowSize(window.m_window, event->screenWidth, event->screenHeight);
  }
  return true;
}
#endif

abcg::OpenGLWindow::~OpenGLWindow() {
  if (m_window != nullptr) {
    if (ImGui::GetCurrentContext() != nullptr) {
      terminateGL();
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplSDL2_Shutdown();
      ImGui::DestroyContext();
    }

    if (m_GLContext != nullptr) {
      SDL_GL_DeleteContext(m_GLContext);
    }
    SDL_DestroyWindow(m_window);
  }
}

abcg::OpenGLSettings abcg::OpenGLWindow::getOpenGLSettings() noexcept {
  return m_openGLSettings;
}

abcg::WindowSettings abcg::OpenGLWindow::getWindowSettings() noexcept {
  return m_windowSettings;
}

void abcg::OpenGLWindow::setOpenGLSettings(
    const OpenGLSettings &openGLSettings) noexcept {
  m_openGLSettings = openGLSettings;
}

void abcg::OpenGLWindow::setWindowSettings(
    const WindowSettings &windowSettings) {
  if (windowSettings.title != m_windowSettings.title) {
    SDL_SetWindowTitle(m_window, windowSettings.title.c_str());
  }

  if (windowSettings.width != m_windowSettings.width ||
      windowSettings.height != m_windowSettings.height) {
    SDL_SetWindowSize(m_window, windowSettings.width, windowSettings.height);
  }

  m_windowSettings = windowSettings;
}

void abcg::OpenGLWindow::handleEvent([[maybe_unused]] SDL_Event &event) {}

void abcg::OpenGLWindow::initializeGL() { glClearColor(0, 0, 0, 1); }

void abcg::OpenGLWindow::paintGL() { glClear(GL_COLOR_BUFFER_BIT); }

void abcg::OpenGLWindow::paintUI() {
  // FPS counter
  if (m_windowSettings.showFPS) {
    float fps = ImGui::GetIO().Framerate;

    static size_t offset{0};
    static double refreshTime = ImGui::GetTime();
    static std::array<float, 150> frames{};

    while (refreshTime < ImGui::GetTime()) {
      const auto refreshFrequency{60.0};
      frames.at(offset) = fps;
      offset = (offset + 1) % frames.size();
      refreshTime += (1.0 / refreshFrequency);
    }

    ImGui::SetNextWindowPos(ImVec2(5, 5));
    ImGui::Begin("FPS", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoFocusOnAppearing);
    std::string label{fmt::format("avg {:.1f} FPS", fps)};
    ImGui::PlotLines("", &frames[0], static_cast<int>(frames.size()),
                     static_cast<int>(offset), label.c_str(), 0.0f,
                     *std::max_element(frames.begin(), frames.end()) * 2,
                     ImVec2(static_cast<float>(frames.size()), 50));
    ImGui::End();
  }

  // Fullscreen button
  if (m_windowSettings.showFullscreenButton) {
#if defined(__EMSCRIPTEN__)
    bool isFullscreenAvailable =
        static_cast<bool>(EM_ASM_INT({ return document.fullscreenEnabled; })) &&
        static_cast<bool>(EM_ASM_INT({ return !isMobile(); }));
    if (isFullscreenAvailable)
#endif
    {
      int windowWidth{};
      int windowHeight{};
      SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

      auto widgetSize{ImVec2(150.0f, 30.0f)};
      auto windowBorder{ImVec2(16.0f, 16.0f)};

      ImGui::SetNextWindowSize(
          ImVec2(widgetSize.x + windowBorder.x, widgetSize.y + windowBorder.y));
      ImGui::SetNextWindowPos(ImVec2(5, static_cast<float>(windowHeight) -
                                            (widgetSize.y + windowBorder.y) -
                                            5));

      ImGui::Begin("Fullscreen", nullptr,
                   ImGuiWindowFlags_NoDecoration |
                       ImGuiWindowFlags_NoBringToFrontOnFocus |
                       ImGuiWindowFlags_NoFocusOnAppearing);

      if (ImGui::Button("Toggle fullscreen", widgetSize)) {
        toggleFullscreen();
      }

      ImGui::End();
    }
  }
}

void abcg::OpenGLWindow::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}

void abcg::OpenGLWindow::terminateGL() {}

GLuint abcg::OpenGLWindow::createProgramFromFile(
    std::string_view pathToVertexShader,
    std::string_view pathToFragmentShader) {
  std::stringstream vertexShaderSource;
  if (std::ifstream stream(pathToVertexShader.data()); stream) {
    vertexShaderSource << stream.rdbuf();
    stream.close();
  } else {
    throw abcg::Exception{abcg::Exception::Runtime(fmt::format(
        "Failed to read vertex shader file {}", pathToVertexShader))};
  }

  std::stringstream fragmentShaderSource;
  if (std::ifstream stream(pathToFragmentShader.data()); stream) {
    fragmentShaderSource << stream.rdbuf();
    stream.close();
  } else {
    throw abcg::Exception{abcg::Exception::Runtime(fmt::format(
        "Failed to read fragment shader file {}", pathToFragmentShader))};
  }

  return createProgramFromString(vertexShaderSource.str(),
                                 fragmentShaderSource.str());
}

GLuint abcg::OpenGLWindow::createProgramFromString(
    std::string_view vertexShaderSource,
    std::string_view fragmentShaderSource) {
  using namespace std::string_literals;

  std::string vsSource{abcg::trimCopy(std::string{vertexShaderSource})};
#if defined(__EMSCRIPTEN__) || defined(__APPLE__)
  // Remove version header, if any
  vsSource =
      std::regex_replace(vsSource, std::regex("#version.*[\r\n|\n|\r]"), "");
  // Add new header
  vsSource = m_GLSLVersion + "\n" + vsSource;
#else
  // Add version header only if missing
  if (!vsSource.starts_with("#version"))
    vsSource = m_GLSLVersion + "\n\n" + vsSource;
#endif

  std::string fsSource{abcg::trimCopy(std::string{fragmentShaderSource})};
#if defined(__EMSCRIPTEN__) || defined(__APPLE__)
  // Remove version header, if any
  fsSource =
      std::regex_replace(fsSource, std::regex("#version.*[\r\n|\n|\r]"), "");

  if (m_openGLSettings.profile == OpenGLProfile::ES) {
    // c++20
    // if (auto regex{std::regex("^precision.*float$", std::multiline)};
    if (auto regex{std::regex("(^|\r\n|\n|\r)precision.*float")};
        !std::regex_search(fsSource, regex)) {
      fsSource = "precision mediump float;\n" + fsSource;
    }
  }
  // Add new header
  fsSource = m_GLSLVersion + "\n" + fsSource;
#else
  // Add version header only if missing
  if (!fsSource.starts_with("#version")) {
    if (m_openGLSettings.profile == OpenGLProfile::ES)
      fsSource = "precision mediump float;\n\n" + fsSource;
    fsSource = m_GLSLVersion + "\n\n" + fsSource;
  }
#endif

  GLint compileStatus{};
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char *vsSourceConstChar = vsSource.c_str();
  glShaderSource(vertexShader, 1, &vsSourceConstChar, nullptr);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus == 0) {
    printShaderInfoLog(vertexShader, "Vertex shader");
    throw abcg::Exception{
        abcg::Exception::Runtime("Failed to compile vertex shader")};
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char *fsSourceConstChar = fsSource.c_str();
  glShaderSource(fragmentShader, 1, &fsSourceConstChar, nullptr);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus == 0) {
    printShaderInfoLog(fragmentShader, "Fragment shader");
    glDeleteShader(vertexShader);
    throw abcg::Exception{
        abcg::Exception::Runtime("Failed to compile fragment shader")};
  }

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);
  GLint linkStatus{};
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == 0) {
    printProgramInfoLog(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    throw abcg::Exception{abcg::Exception::Runtime("Failed to link program")};
  }

  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  return shaderProgram;
}

std::string abcg::OpenGLWindow::getAssetsPath() { return m_assetsPath; }

double abcg::OpenGLWindow::getDeltaTime() const { return m_lastDeltaTime; }

double abcg::OpenGLWindow::getElapsedTime() const {
  return m_windowStartTime.elapsed();
}

void abcg::OpenGLWindow::toggleFullscreen() {
#if defined(__EMSCRIPTEN__)
  EM_ASM(toggleFullscreen(););
#else
  Uint32 windowFlags{SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP};
  bool fullscreen{(SDL_GetWindowFlags(m_window) & windowFlags) != 0u};

  enum class WindowType { Windowed, Fullscreen, FullscreenWindow };
  auto desiredWindowType{fullscreen ? WindowType::Windowed
                                    : WindowType::Fullscreen};

  switch (desiredWindowType) {
    case WindowType::Windowed:
      SDL_SetWindowFullscreen(m_window, 0);
      SDL_SetWindowSize(m_window, m_windowSettings.width,
                        m_windowSettings.height);
      break;
    case WindowType::FullscreenWindow:
      SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
      break;
    case WindowType::Fullscreen:
      SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
      break;
  }
  SDL_ShowCursor(static_cast<int>(fullscreen));
#endif
}

void abcg::OpenGLWindow::handleEvent(SDL_Event &event, bool &done) {
  ImGui_ImplSDL2_ProcessEvent(&event);

  if (event.window.windowID != m_windowID) return;

  if (event.type == SDL_WINDOWEVENT) {
    switch (event.window.event) {
      case SDL_WINDOWEVENT_CLOSE:
        done = true;
        break;
      case SDL_WINDOWEVENT_SIZE_CHANGED: {
        auto &newWidth{event.window.data1};
        auto &newHeight{event.window.data2};
        if (newWidth >= 0 && newHeight >= 0 &&
            (newWidth != m_viewportWidth || newHeight != m_viewportHeight)) {
          m_viewportWidth = newWidth;
          m_viewportHeight = newHeight;
          resizeGL(newWidth, newHeight);
        }
      } break;
      case SDL_WINDOWEVENT_RESIZED: {
        bool fullscreen{
            (SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN) != 0u};
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
        m_viewportWidth = event.window.data1;
        m_viewportHeight = event.window.data2;
        resizeGL(event.window.data1, event.window.data2);
      } break;
    }
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_F11) {
#if defined(__EMSCRIPTEN__)
      bool isFullscreenAvailable =
          static_cast<bool>(
              EM_ASM_INT({ return document.fullscreenEnabled; })) &&
          static_cast<bool>(EM_ASM_INT({ return !isMobile(); }));
      if (isFullscreenAvailable)
#endif
        toggleFullscreen();
    }
  }

  // Won't pass mouse events to the application if ImGUI has captured the
  // mouse
  bool useCustomEventHandler{true};
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

  if (useCustomEventHandler) handleEvent(event);
}

void abcg::OpenGLWindow::initialize(std::string_view basePath) {
  m_deltaTime.restart();
  m_windowStartTime.restart();

  m_assetsPath = std::string(basePath) + "/assets/";

#if defined(__EMSCRIPTEN__)
  if (m_openGLSettings.preserveWebGLDrawingBuffer) {
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
  profile = OpenGLProfile::ES;  // WebGL 2.0
#elif defined(__APPLE__)
  profile = OpenGLProfile::Core;
  majorVersion = 4;
  minorVersion = 1;
#endif

  // Constrain to the range [3.3, 4.0, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6]
  majorVersion = std::min(std::max(majorVersion, 3), 4);
  if (majorVersion == 3) {
    minorVersion = 3;
  } else {
    minorVersion = std::min(std::max(minorVersion, 0), 6);
  }

  m_GLSLVersion +=
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
      majorVersion = 3;
      minorVersion = 0;
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                          SDL_GL_CONTEXT_PROFILE_ES);
      m_GLSLVersion = "#version 300 es";
      break;
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_openGLSettings.depthBufferSize);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_openGLSettings.stencilSize);
  if (m_openGLSettings.samples > 0) {
    // Enable multisample
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // Can be 2, 4, 8 or 16
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_openGLSettings.samples);
  } else {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
  }

  // Create window with graphics context
  while (true) {
    m_window = SDL_CreateWindow(m_windowSettings.title.c_str(),
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                m_windowSettings.width, m_windowSettings.height,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (m_window == nullptr && m_openGLSettings.samples > 0) {
      // Try again, but this time with multisampling disabled
      m_openGLSettings.samples = 0;
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
      fmt::print("Warning: multisampling requested but not supported!\n");
    } else {
      break;
    }
  };

  if (m_window == nullptr) {
    throw abcg::Exception{abcg::Exception::SDL("SDL_CreateWindow failed")};
  }

  m_windowID = SDL_GetWindowID(m_window);

#if defined(__EMSCRIPTEN__)
  emscripten_set_fullscreenchange_callback("#canvas", this, true,
                                           fullscreenchangeCallback);
#endif

  // Create OpenGL context
  m_GLContext = SDL_GL_CreateContext(m_window);
  if (m_GLContext == nullptr) {
    throw abcg::Exception{abcg::Exception::SDL("SDL_GL_CreateContext failed")};
  }

#if !defined(__EMSCRIPTEN__)
  SDL_GL_SetSwapInterval(m_openGLSettings.vsync ? 1 : 0);  // Disable vsync
#endif

#if !defined(__EMSCRIPTEN__)
  if (GLenum err{glewInit()}; GLEW_OK != err) {
    std::string header{"Failed to initialize OpenGL loader: "};
    const auto *const message{
        reinterpret_cast<const char *>(glewGetErrorString(err))};
    throw abcg::Exception{header + message};
  }
  fmt::print("Using GLEW.....: {}\n", glewGetString(GLEW_VERSION));
#endif

  fmt::print("OpenGL vendor..: {}\n", glGetString(GL_VENDOR));
  fmt::print("OpenGL renderer: {}\n", glGetString(GL_RENDERER));
  fmt::print("OpenGL version.: {}\n", glGetString(GL_VERSION));
  fmt::print("GLSL version...: {}\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io{ImGui::GetIO()};
  // Enable keyboard vontrols
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // Enable hamepadv ontrols
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  // For an Emscripten build we are disabling file-system access, so let's not
  // attempt to do a fopen() of the imgui.ini file. You may manually call
  // LoadIniSettingsFromMemory() to load settings from your own storage.
  io.IniFilename = nullptr;

  // Setup our own Dear ImGui style
  setupImGuiStyle(true, 1.0f);

  // Setup platform/renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(m_window, m_GLContext);
  ImGui_ImplOpenGL3_Init(m_GLSLVersion.c_str());

  // Load fonts
  io.Fonts->Clear();

  ImFontConfig fontConfig;
  fontConfig.FontDataOwnedByAtlas = false;
  if (std::array ttf{INCONSOLATA_MEDIUM_TTF};
      io.Fonts->AddFontFromMemoryTTF(ttf.data(), ttf.size(), 16.0f,
                                     &fontConfig) == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Failed to load font file")};
  }

  initializeGL();

  if (io.DisplaySize.x >= 0 && io.DisplaySize.y >= 0) {
    int width{static_cast<int>(io.DisplaySize.x)};
    int height{static_cast<int>(io.DisplaySize.y)};
    m_viewportWidth = width;
    m_viewportHeight = height;
    resizeGL(width, height);
  } else {
    resizeGL(m_windowSettings.width, m_windowSettings.height);
  }
}

void abcg::OpenGLWindow::paint() {
  SDL_GL_MakeCurrent(m_window, m_GLContext);

#if defined(__EMSCRIPTEN__)
  // Force window size in windowed mode
  EmscriptenFullscreenChangeEvent fullscreenStatus{};
  emscripten_get_fullscreen_status(&fullscreenStatus);
  if (fullscreenStatus.isFullscreen == EM_FALSE) {
    SDL_SetWindowSize(m_window, m_windowSettings.width,
                      m_windowSettings.height);
  }
#endif

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  paintUI();
  ImGui::Render();
  paintGL();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(m_window);

  // Cap to 480 Hz
  if (m_deltaTime.elapsed() >= 1.0 / 480.0) {
    m_lastDeltaTime = m_deltaTime.restart();
  } else
    m_lastDeltaTime = 0.0;
}