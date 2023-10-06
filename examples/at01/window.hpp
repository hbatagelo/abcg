#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBOVertices{};
  GLuint m_program{};

  std::default_random_engine m_randomEngine;
  std::array<glm::vec2, 3> const m_points{{{0, 1}, {-1, -1}, {1, -1}}};
  glm::vec2 m_P{};

  void setupModel();
};

#endif