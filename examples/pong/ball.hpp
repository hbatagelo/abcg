#ifndef BALL_HPP_
#define BALL_HPP_

#include <list>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "barLeft.hpp"
#include "barRight.hpp"

class OpenGLWindow;

class Ball {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(BarLeft &barLeft, BarRight &barRight, const GameData &gameData, float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};

  bool m_dead{};
  glm::vec2 m_translation{glm::vec2(0)};
  glm::vec2 m_velocity{glm::vec2(0)};

  bool direction{false};

  float m_scale{0.050f};

  // std::list<Ball> m_balls;
};

#endif