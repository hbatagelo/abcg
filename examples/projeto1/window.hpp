#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>
#include <vector> // Adicionado para armazenar os pontos do gráfico

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
 
private:
std::default_random_engine m_randomEngine; // Mantenha essa linha
  glm::vec2 m_P; // Mantenha esta linha também, se você precisar dela

  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBOVertices{};
  GLuint m_program{};

  float m_a{1.0f}; // Coeficiente angular
  float m_b{0.0f}; // Coeficiente linear
  std::vector<glm::vec2> m_graphPoints; // Armazena os pontos do gráfico

  void setupModel();
  void calculateGraphPoints(); // Método para calcular os pontos do gráfico
};

#endif
