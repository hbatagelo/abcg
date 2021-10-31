#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <array>

#include "abcg.hpp"
#include <imgui.h>

#include <random>

#include "pads.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_objectsProgram{}; // Vamos passar o programa deShader para os pads, então vamos usarum nome diferente pra não termos problema com o m_program dentro dos pads

  int m_viewportWidth{};
  int m_viewportHeight{};
  
  // Estado do jogo
  GameData m_gameData;

  // instância das raquetes
  Pads m_padLeft;
  Pads m_padRight;

  abcg::ElapsedTimer m_restartWaitTimer;

  // std::default_random_engine m_randomEngine;

  // GLuint m_vao{};
  // GLuint m_vboVertices{};
  // GLuint m_vboColors{};

  void restart();
  void update();

  // bool m_showDemoWindow{false};
  // bool m_showAnotherWindow{false};
  std::array<float, 4> m_clearColor{0.0f, 0.0f, 0.0f, 1.00f};
};

#endif