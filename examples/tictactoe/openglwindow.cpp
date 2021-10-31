#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

#include <cppitertools/itertools.hpp>

void OpenGLWindow::initializeGL() {
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 48.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  abcg::glClearColor(0, 0, 0, 1);

  restart();
}

void OpenGLWindow::paintGL() { abcg::glClear(GL_COLOR_BUFFER_BIT); }

void OpenGLWindow::paintUI() {
  const auto appWindowWidth{static_cast<float>(getWindowSettings().width)};
  const auto appWindowHeight{static_cast<float>(getWindowSettings().height)};

  // "Tic-Tac-Toe" window
  {
    ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    const auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize};
    ImGui::Begin("Tic-Tac-Toe", nullptr, flags);

    // Menu
    {
      bool restartSelected{};
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
          ImGui::MenuItem("Restart", nullptr, &restartSelected);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (restartSelected) restart();
    }

    // Static text (win/draw message)
    std::string text;
    switch (m_gameState) {
      case GameState::Play:
        text = fmt::format("{} turn", m_turn ? 'X' : 'O');
        break;
      case GameState::Draw:
        text = "Draw!";
        break;
      case GameState::WinX:
        text = "X is the winner!";
        break;
      case GameState::WinO:
        text = "O is the winner!";
        break;
    }
    ImGui::SetCursorPosX(
        (appWindowWidth - ImGui::CalcTextSize(text.c_str()).x) / 2);
    ImGui::Text("%s", text.c_str());
    ImGui::Spacing();
    ImGui::Spacing();

    // Create game board
    const auto gridHeight{appWindowHeight - 22 - 58 - (m_N * 10) - 58};
    ImGui::PushFont(m_font);
    // For each row
    for (auto i : iter::range(m_N)) {
      ImGui::Columns(m_N);
      // For each column
      for (auto j : iter::range(m_N)) {
        auto offset{i * m_N + j};
        std::string text{fmt::format("{}", m_board.at(offset))};
        ImGui::Button(text.c_str(), ImVec2(-1, gridHeight / m_N));
        if (m_gameState == GameState::Play && m_board.at(offset) == 0) {
          if (ImGui::IsItemClicked()) {
            m_board.at(offset) = m_turn ? 'X' : 'O';
            checkBoard();
            m_turn = !m_turn;
          }
        }
        ImGui::NextColumn();
      }
      if (i < 2) ImGui::Separator();
    }
    ImGui::Columns(1);
    ImGui::PopFont();

    ImGui::Spacing();
    ImGui::Spacing();

    // "Restart game" button
    {
      if (ImGui::Button("Restart game", ImVec2(-1, 50.0f))) {
        restart();
      }
    }

    ImGui::End();
  }
}

void OpenGLWindow::checkBoard() {
  if (m_gameState != GameState::Play) return;

  // Check rows
  for (const auto i : iter::range(m_N)) {
    std::string concatenation{};
    for (const auto j : iter::range(m_N)) {
      const auto offset{i * m_N + j};
      concatenation += m_board.at(offset);
    }
    if (concatenation == std::string(m_N, 'X')) {
      m_gameState = GameState::WinX;
      return;
    }
    if (concatenation == std::string(m_N, 'O')) {
      m_gameState = GameState::WinO;
      return;
    }
  }

  // Check columns
  for (const auto j : iter::range(m_N)) {
    std::string concatenation{};
    for (const auto i : iter::range(m_N)) {
      const auto offset{i * m_N + j};
      concatenation += m_board.at(offset);
    }
    if (concatenation == std::string(m_N, 'X')) {
      m_gameState = GameState::WinX;
      return;
    }
    if (concatenation == std::string(m_N, 'O')) {
      m_gameState = GameState::WinO;
      return;
    }
  }

  // Check main diagonal
  std::string concatenation{};
  for (const auto i : iter::range(m_N)) {
    const auto offset{i * m_N + i};
    concatenation += m_board.at(offset);
  }
  if (concatenation == std::string(m_N, 'X')) {
    m_gameState = GameState::WinX;
    return;
  }
  if (concatenation == std::string(m_N, 'O')) {
    m_gameState = GameState::WinO;
    return;
  }

  // Check inverse diagonal
  concatenation.clear();
  for (const auto i : iter::range(m_N)) {
    const auto offset{i * m_N + (m_N - i - 1)};
    concatenation += m_board.at(offset);
  }
  if (concatenation == std::string(m_N, 'X')) {
    m_gameState = GameState::WinX;
    return;
  }
  if (concatenation == std::string(m_N, 'O')) {
    m_gameState = GameState::WinO;
    return;
  }

  // Check draw
  concatenation.clear();
  for (const auto i : iter::range(m_N)) {
    for (const auto j : iter::range(m_N)) {
      const auto offset{i * m_N + j};
      if (m_board.at(offset) != 0) concatenation += m_board.at(offset);
    }
  }
  if (concatenation.length() == m_N * m_N) {
    m_gameState = GameState::Draw;
  }
}

void OpenGLWindow::restart() {
  m_gameState = GameState::Play;
  m_board.fill(0);
}