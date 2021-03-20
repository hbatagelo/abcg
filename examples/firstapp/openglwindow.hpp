#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <array>
#include <random>
#include <string>

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;

 private:
  std::array<float, 4> m_clearColor{0.906f, 0.910f, 0.918f, 1.0f};

  // objeto para contar o tempo
  abcg::ElapsedTimer m_restartWaitTimer;

  int tabuleiro[3][3];

  int numCorte;
  int maiorMenor;
  std::string maiorMenorStr;
  int score;

  double TempoExtra;
  double tempoJogo;

  // std::default_random_engine m_randomEngine;

  // int gen_number();
  // void incrementaTempo();

  void Sortear();
  void Restart();
};

#endif