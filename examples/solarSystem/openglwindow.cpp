#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <array>
#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

// Custom specialization of std::hash injected in namespace std
namespace std {
  template <>
  struct hash<Vertex> {
    size_t operator()(Vertex const& vertex) const noexcept {
      std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
      return h1;
    }
  };
}

void OpenGLWindow::handleEvent(SDL_Event& ev) {
  if (ev.type == SDL_KEYDOWN) {
    if (ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a)
      m_panSpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d)
      m_panSpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_q) m_truckSpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_e) m_truckSpeed = 1.0f;
  }
  if (ev.type == SDL_KEYUP) {
    if ((ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_q && m_truckSpeed < 0) m_truckSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_e && m_truckSpeed > 0) m_truckSpeed = 0.0f;
  }
}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "lookat.vert",
                                    getAssetsPath() + "lookat.frag");

  // Load model
  loadModelFromFile(getAssetsPath() + "sphere.obj");

  // Generate VBO
  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
               m_vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indices.size(),
               m_indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  glBindVertexArray(0);

  resizeGL(getWindowSettings().width, getWindowSettings().height);
}

void OpenGLWindow::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path =
      getAssetsPath() + "mtl/";  // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig)) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over faces(polygon)
    size_t indexOffset{0};
    for (const auto faceNumber :
         iter::range(shape.mesh.num_face_vertices.size())) {
      // Number of vertices composing face f
      std::size_t numFaceVertices{shape.mesh.num_face_vertices[faceNumber]};
      // Loop over vertices in the face
      std::size_t startIndex{};
      for (const auto vertexNumber : iter::range(numFaceVertices)) {
        // Access to vertex
        tinyobj::index_t index{shape.mesh.indices[indexOffset + vertexNumber]};

        // Vertex coordinates
        startIndex = 3 * index.vertex_index;
        tinyobj::real_t vx = attrib.vertices[startIndex + 0];
        tinyobj::real_t vy = attrib.vertices[startIndex + 1];
        tinyobj::real_t vz = attrib.vertices[startIndex + 2];

        Vertex vertex{};
        vertex.position = {vx, vy, vz};

        // If uniqueVertices doesn't contain this vertex
        if (hash.count(vertex) == 0) {
          // Add this index (size of m_vertices)
          hash[vertex] = m_vertices.size();
          // Add this vertex
          m_vertices.push_back(vertex);
        }

        m_indices.push_back(hash[vertex]);
      }
      indexOffset += numFaceVertices;
    }
  }
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  glUseProgram(m_program);
  glBindVertexArray(m_VAO);

  // Get location of uniform variables (could be precomputed)
  GLint viewMatrixLoc{glGetUniformLocation(m_program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};
  GLint colorLoc{glGetUniformLocation(m_program, "color")};

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_camera.m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_camera.m_projMatrix[0][0]);

  float timer = m_timer.elapsed();

  // Draw Sun
  glm::mat4 model{1.0f};
  model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
  model = glm::rotate(model, glm::radians(15.0f * timer), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.4f));

  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  glUniform4f(colorLoc, 1.0f, 1.0f, 0.0f, 1.0f);
  glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

  Planet mercury;
  mercury.planetColor = glm::vec4{0.41f, 0.41f, 0.6f, 0.6f};
  mercury.planetScale = glm::vec3(0.05f);

  Planet venus;
  venus.planetColor = glm::vec4{0.89f, 0.47f, 0.08f, 0.89f};
  venus.planetScale = glm::vec3(0.08f);

  Planet earth;
  earth.planetColor = glm::vec4{0.0f, 0.0f, 1.0f, 1.0f};
  earth.planetScale = glm::vec3(0.1f);

  Planet mars;
  mars.planetColor = glm::vec4{1.0f, 0.0f, 0.0f, 1.0f};
  mars.planetScale = glm::vec3(0.08f);

  Planet jupiter;
  jupiter.planetColor = glm::vec4{0.54f, 0.36f, 0.34f, 0.55f};
  jupiter.planetScale = glm::vec3(0.29f);

  Planet saturn;
  saturn.planetColor = glm::vec4{0.99f, 0.86f, 0.44f, 1.0f};
  saturn.planetScale = glm::vec3(0.25f);

  Planet uranus;
  uranus.planetColor = glm::vec4{0.84f, 0.99f, 0.99f, 1.0f};
  uranus.planetScale = glm::vec3(0.18f);

  Planet neptune;
  neptune.planetColor = glm::vec4{0.25f, 0.25f, 0.67f, 0.67f};
  neptune.planetScale = glm::vec3(0.15f);

  std::array<Planet, sizeof(Planet)> planets{mercury, venus,  earth,  mars,
                                             jupiter, saturn, uranus, neptune};

  // Draw Planets
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3((1.6f * sin(timer / velocity)), 0.5f,
                                          1.6f * cos(timer / velocity)));
  model = glm::rotate(model, glm::radians(15.0f * timer), glm::vec3(0, 1, 0));
  model = glm::scale(model, planets[planetIndex].planetScale);

  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  glUniform4f(colorLoc, planets[planetIndex].planetColor.r,
              planets[planetIndex].planetColor.g,
              planets[planetIndex].planetColor.b,
              planets[planetIndex].planetColor.a);
  glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  // Create a widgets window
  {
    auto widgetSize{ImVec2(190, 95)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

    {
      // Change planets widget
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Mercurio", "Venus",   "Terra",
                                          "Marte",    "Jupiter", "Saturno",
                                          "Urano",    "Netuno"};

      ImGui::PushItemWidth(100);
      if (ImGui::BeginCombo("Planetas", comboItems.at(currentIndex).c_str())) {
        for (auto index : iter::range(comboItems.size())) {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      planetIndex = currentIndex;

      // Change velocity widget
      ImGui::PushItemWidth(widgetSize.x - 16);
      ImGui::SliderInt("", &velocity, 10, 1, "Velocidade = %d");
      ImGui::PopItemWidth();

      // Button to restart camera
      int click =
          ImGui::Button("Camera Inicial!", ImVec2(widgetSize.x - 16, 25));
      if (click) {
        m_camera.m_eye = glm::vec3(0.0f, 0.5f, 3.5f);
        m_camera.m_at = glm::vec3(0.0f, 0.5f, 0.0f);
        m_camera.m_up = glm::vec3(0.0f, 1.0f, 0.0f);
      }
    }

    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_EBO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
}