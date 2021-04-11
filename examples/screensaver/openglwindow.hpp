#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include "abcg.hpp"
#include "camera.hpp"

struct Vertex {
  glm::vec3 position;

  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_gun_VAO{};
  GLuint m_gun_VBO{};
  GLuint m_gun_EBO{};
  GLuint m_man_VAO{};
  GLuint m_man_VBO{};
  GLuint m_man_EBO{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  int step{-1};

  float timeElapsed{};
  int cameraMovement{1000};

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};

  // std::vector<Vertex> m_vertices;
  // std::vector<GLuint> m_indices;

  std::vector<Vertex> m_man_vertices;
  std::vector<GLuint> m_man_indices;
  std::vector<Vertex> m_gun_vertices;
  std::vector<GLuint> m_gun_indices;

  void loadModelFromFile(std::string_view path, std::vector<Vertex>* vertices,
                         std::vector<GLuint>* indices);
  void update();
  void bindVertexToVAO(GLuint* m_VAO, GLuint* m_VBO, GLuint* m_EBO,
                       GLint* positionAttribute);
  void generateVBO(GLuint* m_VBO, std::vector<Vertex> m_vertices);
  void generateEBO(GLuint* m_EBO, std::vector<GLuint> m_indices);
  void createElement(glm::mat4 model, glm::vec3 pos, float rotate,
                     glm::vec3 scale, glm::vec4 color, GLint* modelMatrixLoc,
                     GLint* colorLoc, std::vector<GLuint>* indices);
};

#endif