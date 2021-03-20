#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <array>

#include <random>

#include <iostream>
#include <string>
#include <sstream>

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  // declaração dos métodos da classe OpenGLWindow que vamos sobrescrever
  void initializeGL() override;
  // initializeGL é onde colocaremos os comandos de inicialização do estado da
  // janela e do OpenGL. Internamente a ABCg chama essa função apenas uma vez no
  // início do programa, após ter inicializado os subsistemas da SDL e o
  // contexto do OpenGL.
  void paintGL() override;
  // paintGL é onde colocaremos todas as funções de desenho do OpenGL.
  // Internamente a ABCg chama essa função continuamente no laço principal da
  // aplicação, uma vez para cada quadro (frame) de exibição.
  void paintUI() override;
  // paintUI é onde colocaremos todas as funções de desenho de widgets da ImGui.
  // Internamente, paintUI é chamado sempre que paintGL é chamado;

 private:
  // An enumeration is a distinct type whose value is restricted to a range of
  // values declarnado uma classe GameState, que possuis os seguinte estados
  enum class GameState { Play, Draw, WinX, WinO };
  // declarando uma constante inteira de valor 5 que será as dimensões do
  // tabuleiro
  static const int m_N{5};  // Board size is m_N x m_N

  // criando um objeto da classe GameState e o inicializando com o valor Play
  GameState m_gameState{GameState::Play};
  // criando uma variavel bool e a inicializando com true, usada para controlar
  // os turnos
  bool m_turn{true};  // true = X, false = O
  // criando um vetor de char de tamanho m_n^2 e o chamando de m_board
  std::array<char, m_N * m_N> m_board{};  // '\0', 'X' or 'O'
  // criando um objeto para a fonte
  ImFont* m_font{};

  //declaração da variavel para ter um timer
  abcg::ElapsedTimer m_restartWaitTimer;

  // declaração dos métodos que serão implementados no CPP
  void checkBoard();
  void restart();
};

#endif
