#ifndef SQUARES_HPP_
#define SQUARES_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "triangle.hpp"

class OpenGLWindow;

class Squares {
 public:
  void initializeGL(GLuint program, int quantity);
  void paintGL();
  void terminateGL();

  void update(const Triangle &triangle, float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  struct Square {
    GLuint m_vao{};
    GLuint m_vbo{};

    float m_angularVelocity{};
    glm::vec4 m_color{1};
    bool m_hit{false};
    int m_polygonSides{};
    float m_scale{};
    glm::vec2 m_translation{glm::vec2(0)};
    glm::vec2 m_velocity{glm::vec2(0)};
  };

  std::list<Square> m_squares;

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};

  Squares::Square createSquare(glm::vec2 translation = glm::vec2(0),
                                     float scale = 0.25f);
};

#endif