#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <vector>
#include <random>

#include "abcg.hpp"
#include "camera.hpp"
#include "ground.hpp"

struct Vertex {
  glm::vec3 position;

  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  int randomInt=0;
  int randomIndex=0;

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};

  float v_box_size{0.4f};
  float size_rate{0.4f};

  glm::vec3 c_green {.0f, 1.f, .0f};
  glm::vec3 c_red   {1.f, .0f, .0f};
  glm::vec3 c_blue  {.0f, .0f, 1.f};
  glm::vec3 random_color{c_green};
  glm::vec3 box_color{c_green};

  // melhoria: criar um vetor com as cores e definir com indice ao inves de if elses

  float max_height{1.5f};
  float v_box_hight{1.5f};
  float vertical_speed{1.5f};
  float is_flying{true};

  Ground m_ground;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  std::default_random_engine m_randomEngine;    // Teste para gerar posicoes aleatorias da bolinha
  std::uniform_real_distribution<float> m_randomDist{1, 10};

  void loadModelFromFile(std::string_view path);
  void update();
};

#endif