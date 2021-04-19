#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

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
}  // namespace std

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
    if (ev.key.keysym.sym == SDLK_SPACE) {
      m_camera.m_eye = glm::vec3(0.0f, 0.5f, 2.5f);
      m_camera.m_at = glm::vec3(0.0f, 0.5f, 0.0f);
      m_camera.m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    }
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

  m_starsProgram = createProgramFromFile(getAssetsPath() + "stars.vert",
                                         getAssetsPath() + "stars.frag");
  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "deathstar.vert",
                                    getAssetsPath() + "deathstar.frag");
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Load model
  // man
  loadModelFromFile(getAssetsPath() + "source/bunny.obj", &m_man_vertices,
                    &m_man_indices);
  // gun
  // loadModelFromFile(getAssetsPath() + "Handgun_obj.obj", &m_gun_vertices,
  // &m_gun_indices);

  // Generate VBO
  generateVBO(&m_man_VBO, m_man_vertices);
  // generateVBO(&m_gun_VBO, m_gun_vertices);

  // Generate EBO
  generateEBO(&m_man_EBO, m_man_indices);
  // generateEBO(&m_gun_EBO, m_gun_indices);

  // Create VAO
  glGenVertexArrays(1, &m_man_VAO);

  // Bind vertex attributes to current VAO
  bindVertexToVAO(&m_man_VAO, &m_man_VBO, &m_man_EBO, &positionAttribute);

  // End of binding to current VAO
  glBindVertexArray(0);

  // Create VAO
  // glGenVertexArrays(1, &m_gun_VAO);

  // Bind vertex attributes to current VAO
  // bindVertexToVAO(&m_gun_VAO, &m_gun_VBO, &m_gun_EBO, &positionAttribute);

  // End of binding to current VAO
  // glBindVertexArray(0);

  resizeGL(getWindowSettings().width, getWindowSettings().height);
  m_starLayers.initializeGL(m_starsProgram, 25);
}

void OpenGLWindow::bindVertexToVAO(GLuint* m_VAO, GLuint* m_VBO, GLuint* m_EBO,
                                   GLint* positionAttribute) {
  glBindVertexArray(*m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, *m_VBO);
  glEnableVertexAttribArray(*positionAttribute);
  glVertexAttribPointer(*positionAttribute, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_EBO);
}

void OpenGLWindow::generateVBO(GLuint* m_VBO, std::vector<Vertex> m_vertices) {
  glGenBuffers(1, &(*m_VBO));
  glBindBuffer(GL_ARRAY_BUFFER, *m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
               m_vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLWindow::generateEBO(GLuint* m_EBO, std::vector<GLuint> m_indices) {
  glGenBuffers(1, &(*m_EBO));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indices.size(),
               m_indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLWindow::loadModelFromFile(std::string_view path,
                                     std::vector<Vertex>* vertices,
                                     std::vector<GLuint>* indices) {
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

  (*vertices).clear();
  (*indices).clear();

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
          hash[vertex] = (*vertices).size();
          // Add this vertex
          (*vertices).push_back(vertex);
        }

        (*indices).push_back(hash[vertex]);
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

  m_starLayers.paintGL();

  glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  GLint viewMatrixLoc{glGetUniformLocation(m_program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};
  GLint colorLoc{glGetUniformLocation(m_program, "color")};

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_camera.m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_camera.m_projMatrix[0][0]);

  // gun.obj scale to stay with camera in same position
  glm::vec3 gun_scale{1.0f};
  // man.obj scale to stay with camera in same position
  glm::vec3 man_scale{1.0f};
  // man.obj middle position
  glm::vec3 man_pos{0.0f, 0.0f, 0.0f};

  // start man vertex array use
  glBindVertexArray(m_man_VAO);
  // Draw left item
  // createElement(glm::mat4{0.1f},
  // glm::vec3{man_pos.x - 0.75f, man_pos.y, man_pos.z + 0.5f},
  // glm::radians(+45.0f), man_scale,
  // glm::vec4(0.57f, 0.124f, 0.75f, 1.25f), &modelMatrixLoc,
  // &colorLoc, &m_man_indices);

  // Draw middle item
  createElement(glm::mat4{0.1f}, man_pos, glm::radians(0.0f), man_scale,
                glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), &modelMatrixLoc, &colorLoc,
                &m_man_indices);

  // Draw right item
  // createElement(glm::mat4{0.1f},
  // glm::vec3(man_pos.x + 0.75f, man_pos.y, man_pos.z + 0.5f),
  // glm::radians(-45.0f), man_scale,
  // glm::vec4(0.57f, 0.124f, 0.75f, 1.25f), &modelMatrixLoc,
  // &colorLoc, &m_man_indices);
  // end man vertex array use
  glBindVertexArray(0);

  // start gun vertex array use
  // glBindVertexArray(m_gun_VAO);
  // Draw gun item
  // createElement(
  // glm::mat4{1.0f},
  // glm::vec3{man_pos.x - 1.05f, man_pos.y, man_pos.z + 0.05f},
  // glm::radians(180.0f), gun_scale, glm::vec4(1.0f), &modelMatrixLoc,
  // &colorLoc, &m_gun_indices);

  // end gun vertex array use
  // glBindVertexArray(0);
  glUseProgram(0);
}

void OpenGLWindow::createElement(glm::mat4 model, glm::vec3 pos, float rotate,
                                 glm::vec3 scale, glm::vec4 color,
                                 GLint* modelMatrixLoc, GLint* colorLoc,
                                 std::vector<GLuint>* indices) {
  model = glm::translate(model, pos);
  model = glm::rotate(model, rotate, glm::vec3(0, 1, 0));
  model = glm::scale(model, scale);

  glUniformMatrix4fv((*modelMatrixLoc), 1, GL_FALSE, &model[0][0]);
  glUniform4f((*colorLoc), color.x, color.y, color.z, color.w);
  glDrawElements(GL_TRIANGLES, (*indices).size(), GL_UNSIGNED_INT, nullptr);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  { timeElapsed += 1000 / ImGui::GetIO().Framerate; }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_program);
  glDeleteProgram(m_starsProgram);
  // glDeleteBuffers(1, &m_gun_EBO);
  // glDeleteBuffers(1, &m_gun_VBO);
  // glDeleteVertexArrays(1, &m_gun_VAO);
  glDeleteBuffers(1, &m_man_EBO);
  glDeleteBuffers(1, &m_man_VBO);
  glDeleteVertexArrays(1, &m_man_VAO);
  m_starLayers.terminateGL();
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_starLayers.update(deltaTime);
}