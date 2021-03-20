#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <array>
#include <glm/vec4.hpp>
#include <random>

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_vao{};
  GLuint m_vboPositions{};
  GLuint m_vboColors{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;

  std::array<glm::vec4, 3> m_vertexColors{glm::vec4{0.36f, 0.83f, 1.00f, 1.0f},
                                          glm::vec4{0.63f, 0.00f, 0.61f, 1.0f},
                                          glm::vec4{1.00f, 0.69f, 0.30f, 1.0f}};

  void setupModel();
};

#endif