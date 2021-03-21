#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <array>
#include <random>
#include <string>

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
  // paintUI é onde colocaremos todas as funções de desenho de widgets da
  // ImGui. Internamente, paintUI é chamado sempre que paintGL é chamado;

 private:
  std::array<float, 4> m_clearColor{0.906f, 0.910f, 0.918f, 1.0f};

  // objeto para contar o tempo
  abcg::ElapsedTimer m_restartWaitTimer;

  // criando a grade onde os numeros vao aparecer
  int tabuleiro[3][3];

  // declarando o numero de corte
  int numCorte;
  // declarando variavel que vai decidir se os numeros precisam ser maiores o
  // menores que o numero de corte
  int maiorMenor;
  // string usada para montar o display com 'maior' ou 'menor'
  std::string maiorMenorStr;
  // inteiro para somar o score
  int score;

  // o tempo que ganhamos ao acetar os números
  double TempoExtra;
  // o tempo que falta para o jogo acabar
  double tempoJogo;

  // std::default_random_engine m_randomEngine;

  // int gen_number();
  // void incrementaTempo();

  // sortear novamente os numeros no tabuleiro
  void Sortear();
  // recomeçar o jogo
  void Restart();
};

#endif