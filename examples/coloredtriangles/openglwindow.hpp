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
  GLuint m_vao{};           // Vertex array object (o estado da configuração de como os vertices serão lidos)
  GLuint m_vboPositions{};  // Posições dos vértices
  GLuint m_vboColors{};     // Cores dos vértices
  GLuint m_program{};       // comportamento do pipeline de renderização (é o par de shaders que vem da GPU)

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;

  std::array<glm::vec4, 3> m_vertexColors{glm::vec4{0.36f, 0.83f, 1.00f, 1.0f},
                                          glm::vec4{0.63f, 0.00f, 0.61f, 1.0f},
                                          glm::vec4{1.00f, 0.69f, 0.30f, 1.0f}};

  void setupModel();
};

#endif