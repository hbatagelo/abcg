#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <array>
#include <vector>

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  // Desenha a tela inicial do programa
  void initializeGL() override;

  // Desenha continuamente os elementos (chamada a cada Frame)
  void paintGL() override;

  // Desenha os widgets (botões, menus, caixas ...)
  void paintUI() override;

  // Ajusta a ViewPort para o tamanho da window
  // void resizeGL(int width, int height) override;
  
  // Finaliza a execução do programa (oposto a initializaGL)
//   void terminateGL() override;

 // Definição de todas as varíaveis que serão usadas
 private:
  // A tela voltará para a m_clear_color antes do início da composição de cada frame
  std::array<float, 4> m_clearColor{0.906f, 0.910f, 0.918f, 1.00f}; //float m_clearColor[4] = {0.906f, 0.910f, 0.918f, 1.0f};

  // Define de quem é o turno atual
  bool is_x_turn{true}; 

  // Possíveis estados do jogo (em jogo, empate, 'X' vence, 'O' vence)
  enum class GameState { Play, Draw, WinX, WinO };
  GameState m_gameState{GameState::Play};


  static const int b_side{3};  // Quantos quadrados por lado
  std::array<char,b_side * b_side> board{'\0','\0','\0','\0','\0','\0','\0','\0','\0'};

  void checkBoard();
  void restart();

};

#endif