#include <fmt/core.h>

#include "openglwindow.hpp"

#include <imgui.h>

void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  abcg::glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
                     m_clearColor[3]);
  // Clear the color buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::OpenGLWindow::paintUI();

  // Our own ImGui widgets go below
  {
    // Window begin
    ImGui::Begin("Hello, First App!");

    // Static text
    auto windowSettings{getWindowSettings()};
    ImGui::Text("Current window size: %dx%d (in windowed mode)",
                windowSettings.width, windowSettings.height);

    // Slider from 0.0f to 1.0f
    static float f{};
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

    // ColorEdit to change the clear color
    ImGui::ColorEdit3("clear color", m_clearColor.data());

    // More static text
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // Window end
    ImGui::End();
  }
}