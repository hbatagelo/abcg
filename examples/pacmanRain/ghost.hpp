#ifndef GHOST_HPP_
#define GHOST_HPP_

#include "abcg.hpp"
#include "gamedata.hpp"

class Pacmans;
class OpenGLWindow;

class Ghost {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();
  void update(const GameData &gameData);

 private:
  friend OpenGLWindow;
  friend Pacmans;

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec4 m_color{1.0f, 0.0f, 0.0f, 1.0f};
  float m_scale{0.5f};
  glm::vec2 m_translation{glm::vec2(0)};
  glm::vec2 m_velocity{glm::vec2(0)};

  abcg::ElapsedTimer m_ghostTimer;
};
#endif