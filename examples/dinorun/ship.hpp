#ifndef SHIP_HPP_
#define SHIP_HPP_

#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"

// class Asteroids;
class Bullets;
class OpenGLWindow;
class StarLayers;

class Ship {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void update(const GameData &gameData, float deltaTime);
  void setRotation(float rotation) { m_rotation = rotation; }

 private:
  glm::vec4 getRandomVertexColor();
  // friend Asteroids;
  friend Bullets;
  friend OpenGLWindow;
  friend StarLayers;

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec4 m_color{0.63f, 0.00f, 0.61f, 1.0f};
  float m_rotation{};
  float m_scale{0.125f};
  bool randomColors{true};

  glm::vec2 m_translation{glm::vec2(0)};
  glm::vec2 m_velocity{glm::vec2(0)};

  std::default_random_engine m_randomEngine;

  abcg::ElapsedTimer m_trailBlinkTimer;
  abcg::ElapsedTimer m_bulletCoolDownTimer;
};

#endif