#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    const std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
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
    if (ev.key.keysym.sym == SDLK_j) box_color = c_red;
    if (ev.key.keysym.sym == SDLK_k) box_color = c_green;
    if (ev.key.keysym.sym == SDLK_l) box_color = c_blue;
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
  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);


  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "lookat.vert",
                                    getAssetsPath() + "lookat.frag");

  m_ground.initializeGL(m_program);

  // Load model
  loadModelFromFile(getAssetsPath() + "cube.obj");

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices[0]) * m_indices.size(), m_indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  const GLint positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  resizeGL(getWindowSettings().width, getWindowSettings().height);
}

void OpenGLWindow::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
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
    // Loop over indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      const tinyobj::index_t index{shape.mesh.indices.at(offset)};

      // Vertex position
      const int startIndex{3 * index.vertex_index};
      const float vx{attrib.vertices.at(startIndex + 0)};
      const float vy{attrib.vertices.at(startIndex + 1)};
      const float vz{attrib.vertices.at(startIndex + 2)};

      Vertex vertex{};
      vertex.position = {vx, vy, vz};

      // If hash doesn't contain this vertex
      if (hash.count(vertex) == 0) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}

void OpenGLWindow::paintGL() {
  
  if(gameState == 0 ){  // Só modifica a tela se o estado for 0, ou seja, estado = jogar!

    update();

  }
  else{

    // Cronometro de tempo de estados de win e game over
    const float deltaTime{static_cast<float>(getDeltaTime())};
    stateTimer += deltaTime;

  }

  if(stateTimer >= 5.0f){  // Apos 5 segs de tela de win/game over, reinicia o game!

    stateTimer = 0.0f;
    restart();

  }
  

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);

  abcg::glBindVertexArray(m_VAO);

  // Draw Blue Cube
  glm::mat4 model{1.0f};
  model = glm::translate(model, glm::vec3(0.0f, v_box_size*1.463f, 0.0f));
  model = glm::scale(model, glm::vec3(0.4f+0.5f*(0.4f-v_box_size) , v_box_size, 0.4f+0.5f*(0.4f-v_box_size)));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, random_color[0]-0.5f, random_color[1]-0.5f, random_color[2]-0.5f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);
  

  // Draw red Cube
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(0.0f, 0.3f*1.15f+2*(v_box_size*1.463f)+v_box_hight, 0.0f));
  model = glm::scale(model, glm::vec3(0.3f , 0.3f, 0.3f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, box_color[0], box_color[1], box_color[2], 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);


  abcg::glBindVertexArray(0);

  // Draw ground
  m_ground.paintGL();

  abcg::glUseProgram(0);
}

//void OpenGLWindow::paintUI() { abcg::OpenGLWindow::paintUI(); }     // trocar pelo paintUI do asteroids!


void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    const auto size{ImVec2(300, 85)};
    const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                               (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (gameState == 1) {
      ImGui::Text("Game Over!");
    } else if (gameState == 2) {
      ImGui::Text("*You Win!*");
    }

    ImGui::PopFont();
    ImGui::End();
  }

}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  m_ground.terminateGL();

  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);


  if (is_flying){
    if(v_box_hight < 0.0f){
      vertical_speed *= -1;
      v_box_hight = 0.0f;
      max_height = max_height - (max_height * 0.1f);
      
      if (box_color != random_color){
        gameState = 1
      }

      is_flying = false;
    }
    if (v_box_hight > max_height){
      vertical_speed *= -1;
      v_box_hight = max_height;
    }
    v_box_hight += vertical_speed*deltaTime;
  } else {
    if(v_box_size < 0.2f){
      size_rate *= -1;
      v_box_size = 0.2f;
     /* ******* ESCOLHER AQUI NOVA COR ENTRE [c_green,c_red, c_blue]********* */
      m_randomEngine.seed(std::chrono::steady_clock::now().time_since_epoch().count());

      randomInt = m_randomDist(m_randomEngine);
      randomIndex = randomInt % 3;

      // logica para definir cor aleatoria
      if(randomIndex == 0)
      {
        random_color = c_green;
      }
      else if(randomIndex == 1)
      {
        random_color = c_red;
      }
      else{
        random_color = c_blue;
      }

    }
    if (v_box_size > 0.4f){
      size_rate *= -1;
      v_box_size = 0.4f;
      is_flying = true;

      // logica simples para testar telas de gameover e win
      // inserir aqui futuramente criterios para definir win e game over states
      //jumpCount += 1;

      //if (jumpCount == 10){
      
      //  gameState = 1;

      //}
      if (jumpCount == 13){
        gameState = 2;
      }
      //else if(jumpCount >= 30){
       // gameState = 0;
      //  jumpCount = 0;
      }
    }
    v_box_size += size_rate*deltaTime;
  }
}

void OpenGLWindow::restart() {

  // define o estado para jogando
  gameState = 0;

  // redefinir variaveis para estado inicial
  jumpCount = 0;

  v_box_hight = 1.5f;
  v_box_size = 0.4f;
  max_height = 1.5f;

  vertical_speed = 1.5f;

  is_flying = true;

}
