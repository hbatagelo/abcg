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

void printBoard(char board[]) {
  fmt::print("board = ");
  for (int j = 0; j < 9; j++) {
    fmt::print(" {} ", board[j]);
  }
  fmt::print("\n\n");
}

void fillBoard(char board[]) {
  for (int j = 0; j < 9; j++) {
    if (board[j] == ' ') board[j] = '-';
  }
}

bool resetBoard(char board[], char btns[]) {
  for (int j = 0; j < 9; j++) {
    board[j] = ' ';
    btns[j] = 0;
  }
  return false;
}

bool checkMove(char *player, char board[]) {
  printBoard(board);

  if (((board[0] == *player && board[1] == *player) && board[2] == *player) ||
      ((board[0] == *player && board[4] == *player) && board[8] == *player) ||
      ((board[3] == *player && board[4] == *player) && board[5] == *player) ||
      ((board[6] == *player && board[7] == *player) && board[8] == *player) ||
      ((board[2] == *player && board[4] == *player) && board[6] == *player) ||
      ((board[0] == *player && board[3] == *player) && board[6] == *player) ||
      ((board[1] == *player && board[4] == *player) && board[7] == *player) ||
      ((board[2] == *player && board[6] == *player) && board[8] == *player)) {
    fillBoard(board);
    return true;
  }

  return false;
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::OpenGLWindow::paintUI();
  // Our own ImGui widgets go below
  {
    ImGui::SetNextWindowSize(ImVec2(560, 500));
    ImGui::SetNextWindowPos(ImVec2(80, 5), ImGuiCond_FirstUseEver);
    // Window begin
    ImGui::Begin("Tic-Tac-Toe");

    ImGui::ShowDemoWindow();

    // UX stuff
    static bool h_borders = true;
    static bool v_borders = true;
    static int columns_count = 3;
    const int lines_count = 3;
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);

    /* board controller

  [0,1,2,3,4,5,6,7,8] =
  [
    0,1,2
    3,4,5
    6,7,8
  ]

*/
    static char board[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    static char btns[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    static int clicked = 0;

    static char xis = 'X';
    static char bolinha = 'O';
    static bool endGame = false;

    static char *pTurn = &xis;

    ImGui::AlignTextToFramePadding();
    if (endGame) {
      ImGui::Text("End game");
      if (*pTurn == 'O')
        ImGui::Text("X wins!");
      else
        ImGui::Text("O wins!");
    } else
      ImGui::Text("It's %c turn", *pTurn);

    ImGui::Columns(columns_count, NULL, v_borders);

    for (int i = 0; i < columns_count * lines_count; i++) {
      if (h_borders && ImGui::GetColumnIndex() == 0) ImGui::Separator();

      ImGui::Button(std::string(1, board[i]).c_str(), ImVec2(165, 75));
      if (ImGui::IsItemClicked() && endGame != true) {
        clicked++;
        if (btns[i] == 0) {
          if (*pTurn == 'X') {
            board[i] = 'X';
            endGame = checkMove(pTurn, board);
            pTurn = &bolinha;

          } else {
            board[i] = 'O';
            endGame = checkMove(pTurn, board);
            pTurn = &xis;
          }
        }

        btns[i] = 1;
        clicked = 0;
      }
      ImGui::NextColumn();
    }
    ImGui::Separator();

    ImGui::Columns(1);
    ImGui::NewLine();
    if (ImGui::Button("Reset", ImVec2(540, 50))) {
      endGame = resetBoard(board, btns);
      pTurn = &xis;
    }
    ImGui::NewLine();
    // Static text
    auto windowSettings{getWindowSettings()};
    ImGui::Text("Current window size: %dx%d (in windowed mode)",
                windowSettings.width, windowSettings.height);

    // More static text
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // Window end
    ImGui::End();
  }
}