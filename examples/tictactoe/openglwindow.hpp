#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <array>

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;

 private:
  enum class GameState { Play, Draw, WinX, WinO };
  static const int m_N{3};  // Board size is m_N x m_N

  GameState m_gameState{GameState::Play};
  bool m_turn{true};                      // true = X, false = O
  std::array<char, m_N * m_N> m_board{};  // '\0', 'X' or 'O'

  ImFont* m_font{};

  void checkBoard();
  void restart();
};

#endif
