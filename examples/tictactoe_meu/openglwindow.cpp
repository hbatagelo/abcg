#include <fmt/core.h>

#include <bits/stdc++.h>

#include "openglwindow.hpp"

#include <imgui.h>

void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
  
  // Set the clear color
  abcg::glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
                     m_clearColor[3]);
}

void OpenGLWindow::paintGL() {
  // Clear the color buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
//   abcg::OpenGLWindow::paintUI();

  // Our own ImGui widgets go below
  {
    // Window begin
    ImGui::Begin("Welcome to the Tic Tac Toe game");

    // Static text
    ImGui::Text("Current Player: %c", active_player);

    // Squares
    for (int i=0; i<9; ++i){
        ImGui::Button(board[i], ImVec2(100, 50));
        if (ImGui::IsItemClicked() && board[i] == ' '){
            board[i] = active_player;
            if (active_player == 'X'){
                active_player = 'O';
            } else {
                active_player = 'X';
            }
        }
        if (i%3 != 2){
            ImGui::SameLine();
        }
    }

//     // Slider from 0.0f to 1.0f
//     ImGui::SliderFloat("float", &slider_value, 0.0f, 1.0f);

//     // ColorEdit to change the clear color
//     ImGui::ColorEdit3("clear color", m_clearColor.data());

//     // More static text
//     ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
//                 1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

//     // 100x50 button
//     if (ImGui::Button("Press me!", ImVec2(100, 50))) {
//     fmt::print("Button pressed.\n");
//     }

//     // Nx50 button, where N is the remaining width available
//     ImGui::Button("Press me!", ImVec2(-1, 50));
//     // See also IsItemHovered, IsItemActive, etc
//     if (ImGui::IsItemClicked()) {
//     fmt::print("Button pressed.\n");
//     }

//     static bool enabled{true};
//     ImGui::Checkbox("Some option", &enabled);
//     // fmt::print("The checkbox is {}\n", enabled ? "enabled" : "disabled");

//     static std::size_t currentIndex{};
//     std::vector<std::string> comboItems{"AAA", "BBB", "CCC"};

//     if (ImGui::BeginCombo("Combo box", comboItems.at(currentIndex).c_str())) {
//     for (auto index{0u}; index < comboItems.size(); ++index) {
//         const bool isSelected{currentIndex == index};
//         if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
//         currentIndex = index;

//         // Set the initial focus when opening the combo (scrolling + keyboard
//         // navigation focus)
//         if (isSelected) ImGui::SetItemDefaultFocus();
//     }
//     ImGui::EndCombo();
//     }

//     // fmt::print("Selected combo box item: {}\n", comboItems.at(currentIndex));

//     // Window end
//     ImGui::End();
//   }
  
//   // Create a new floating menu
//   {
//     ImGui::SetNextWindowSize(ImVec2(300, 100));
//     auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize};
//     ImGui::Begin("Window with menu", nullptr, flags);
//     {
//         bool save{};
//         static bool showCompliment{};  // Hold state

//         // Menu Bar
//         if (ImGui::BeginMenuBar()) {
//             // File menu
//             if (ImGui::BeginMenu("File")) {
//                 ImGui::MenuItem("Save", nullptr, &save);
//                 ImGui::EndMenu();
//             }
//             // View menu
//             if (ImGui::BeginMenu("View")) {
//                 ImGui::MenuItem("Show Compliment", nullptr, &showCompliment);
//                 ImGui::EndMenu();
//             }
//             ImGui::EndMenuBar();
//         }

//         if (save) {
//             fmt::print("Saving fake File ...\n");
//         }

//         if (showCompliment) {
//             ImGui::Text("You're a beautiful person.");
//         }
//     }
    ImGui::End();
  }
}

// void OpenGLWindow::terminateGL() {
//     fmt::print("Closing Application ...\n");
// }