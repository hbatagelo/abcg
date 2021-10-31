#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <random>

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_vao{};
  GLuint m_vboPositions{};
  GLuint m_vboColors{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  // Gerador de números aleatórios
  std::default_random_engine m_randomEngine;

  // Definirá o intervalor em ms entre as renderizações dos polígonos
  int m_delay{200};
  // ElapsedTimer é um temporizador
  // ---- restart() Reinicia a contagem do tempo de existência do objeto
  // ---- elapsed() Retorna o tempo desde a criação do objeto ou chamada do restart()
  abcg::ElapsedTimer m_elapsedTimer;

  void setupModel(int sides);
};

#endif