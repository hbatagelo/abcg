#ifndef AMMOBAR_HPP_
#define AMMOBAR_HPP_

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class AmmoBar {
   public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void update(const GameData &gameData);

 private:
  friend OpenGLWindow;
  

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};


  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec4 m_color{1};
  //float m_scale{0.125f};

};

#endif