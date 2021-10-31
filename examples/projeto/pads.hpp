#ifndef PADS_HPP_
#define PADS_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class Pads {
 public:
  void initializeGL(GLuint program, glm::vec2 startPosition, int side);
  void paintGL();
  void terminateGL();

  void update(GameData gamedata, float deltaTime);

 private:
  friend OpenGLWindow;

  // glm::vec2 m_startPosition{};
  int m_side{};
  GLuint m_program{};
  GLint m_colorLoc{};
  // GLint m_rotationLoc{};
  // GLint m_translationLoc{};
  // GLint m_scaleLoc{};
  GLuint m_vao{};
  GLuint m_vbo{};

  glm::vec2 m_padPosition{};
  // float m_angularVelocity{};
  glm::vec4 m_color{1};

  // std::default_random_engine m_randomEngine;
  // std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};

  // Pads::Pad createPad(glm::vec2 padInitialPosition = glm::vec2(0));
};

#endif