#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <array>

#include "abcg.hpp"
#include <imgui.h>

#include "gamedata.hpp"
#include <random>

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;
  void DrawBall(int ballCenterX, int ballCenterY);
  

 private:
  GameData m_gameData;
  GLuint m_vao{};
  GLuint m_vboVertices{};
  GLuint m_vboColors{};
  GLuint m_vboVertices2{};
  GLuint m_vboColors2{};
  GLuint m_program{};
  GLuint m_bola;

  GLint m_rotationLoc{};
  GLint m_scaleLoc{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;

  float ratio = 16.0f/9.0f;

  float pad_leftCenter=0.0f;
  float pad_rightCenter=0.0f;

  float pad_leftSpeed=0.0f;
  float pad_rightSpeed=0.0f;

  float ball_xCenter=0.0f;
  float ball_yCenter=0.0f;

  float ball_xSpeed = 0.0f;
  float ball_ySpeed = 0.0f;

  bool m_showDemoWindow{false};
  bool m_showAnotherWindow{false};
  std::array<float, 4> m_clearColor{0.0f, 0.0f, 0.0f, 0.00f};
};

#endif