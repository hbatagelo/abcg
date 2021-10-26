#ifndef STARLAYERS_HPP_
#define STARLAYERS_HPP_

#include <array>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "ship.hpp"

class OpenGLWindow;

class StarLayers {
 public:
  void initializeGL(GLuint program, int quantity);
  void paintGL();
  void terminateGL();

  void update(const Ship &ship, float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_pointSizeLoc{};
  GLint m_translationLoc{};

  struct StarLayer {
    GLuint m_vao{};
    GLuint m_vbo{};

    float m_pointSize{};
    int m_quantity{};
    glm::vec2 m_translation{glm::vec2(0)};
  };

  std::array<StarLayer, 5> m_starLayers;

  std::default_random_engine m_randomEngine;
};

#endif