#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
               m_clearColor[3]);
  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::paintUI() {
  ImGui::SetNextWindowSize(ImVec2(300, 100));
  auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize};
  ImGui::Begin("Window with menu", nullptr, flags);
  {
    bool save{};
    static bool showCompliment{};  // Hold state


    //Exemplo de Menubar
    // Menu Bar
    if (ImGui::BeginMenuBar()) {
      // File menu
      if (ImGui::BeginMenu("File")) {
        ImGui::MenuItem("Save", nullptr, &save);
        ImGui::EndMenu();
      }
      // View menu
      if (ImGui::BeginMenu("View")) {
        ImGui::MenuItem("Show Compliment", nullptr, &showCompliment);
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    if (save) {
      // Save file...
    }

    if (showCompliment) {
      ImGui::Text("You're a beautiful person.");
    }
  }
  ImGui::End();
}
