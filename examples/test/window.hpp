#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "model.hpp"
#include "trackball.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

  glm::vec3 position{0.0f, 0.0f, 0.0f};

private:
  glm::ivec2 m_viewportSize{};

  Model m_model;
  int m_trianglesToDraw{};

  TrackBall m_trackBallModel;
  TrackBall m_trackBallLight;
  float m_zoom{};

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  // Shaders
  std::vector<char const *> m_shaderNames{
      "cubereflect", "cuberefract", "normalmapping", "texture", "blinnphong",
      "phong",       "gouraud",     "normal",        "depth"};
  std::vector<GLuint> m_programs;
  int m_currentProgramIndex{};

  // Mapping mode
  // 0: triplanar; 1: cylindrical; 2: spherical; 3: from mesh
  int m_mappingMode{};

  // Light and material properties
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{};
  glm::vec4 m_Kd{};
  glm::vec4 m_Ks{};
  float m_shininess{};

  // Skybox
  std::string const m_skyShaderName{"skybox"};
  GLuint m_skyVAO{};
  GLuint m_skyVBO{};
  GLuint m_skyProgram{};

  // Ball
  std::string const m_BallShaderName{"ball"};
  GLuint m_ballVAO{};
  GLuint m_ballVBO{};
  GLuint m_ballEBO{};
  GLuint m_ballProgram{};
  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
  int m_verticesToDraw{};


  // clang-format off
  std::array<glm::vec3, 36> const m_skyPositions{{
      // Front
      {-1, -1, +1}, {+1, -1, +1}, {+1, +1, +1},
      {-1, -1, +1}, {+1, +1, +1}, {-1, +1, +1},
      // Back
      {+1, -1, -1}, {-1, -1, -1}, {-1, +1, -1},
      {+1, -1, -1}, {-1, +1, -1}, {+1, +1, -1},
      // Right
      {+1, -1, -1}, {+1, +1, -1}, {+1, +1, +1},
      {+1, -1, -1}, {+1, +1, +1}, {+1, -1, +1},
      // Left
      {-1, -1, +1}, {-1, +1, +1}, {-1, +1, -1},
      {-1, -1, +1}, {-1, +1, -1}, {-1, -1, -1},
      // Top
      {-1, +1, +1}, {+1, +1, +1}, {+1, +1, -1},
      {-1, +1, +1}, {+1, +1, -1}, {-1, +1, -1},
      // Bottom
      {-1, -1, -1}, {+1, -1, -1}, {+1, -1, +1},
      {-1, -1, -1}, {+1, -1, +1}, {-1, -1, +1}}};
  // clang-format on

  void createSkybox();
  void renderSkybox();
  void destroySkybox() const;
  void createBall();
  void renderBall();
  void destroyBall() const;
  void loadModel(std::string_view path);
  void loadModelFromFile(std::string_view path);
  void standardize();
};

#endif