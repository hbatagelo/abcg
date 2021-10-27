#ifndef BAR_HPP_
#define BAR_HPP_

#include "abcg.hpp"
#include "gamedata.hpp"

/*
class Asteroids;
class Bullets;
class StarLayers;
*/
class OpenGLWindow;
class Ball;

class BarLeft {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  
  void update(const GameData &gameData, float deltaTime);
  

 private:
  friend OpenGLWindow;  
  friend Ball;

/*  friend Asteroids;
  friend Bullets;
  friend StarLayers;
*/
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec4 m_color{1};
  float m_rotation{};
  float m_scale{1.000f};
  glm::vec2 m_translation{glm::vec2(0)};
  glm::vec2 m_velocity{glm::vec2(0)};

  abcg::ElapsedTimer m_trailBlinkTimer;
  abcg::ElapsedTimer m_ballCoolDownTimer;
};

#endif