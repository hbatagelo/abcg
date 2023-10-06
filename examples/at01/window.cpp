#include "window.hpp"
void drawCircle(glm::vec2 center, float radius, glm::vec4 color);

void Window::onCreate() {
  auto const *vertexShader{R"gl(#version 300 es
    layout(location = 0) in vec2 inPosition;

    void main() { 
      gl_PointSize = 2.0;
      gl_Position = vec4(inPosition, 0, 1); 
    }
  )gl"};

  auto const *fragmentShader{R"gl(#version 300 es
    precision mediump float;

    out vec4 outColor;

    void main() { outColor = vec4(1); }
  )gl"};

  // Create shader program
  m_program = abcg::createOpenGLProgram(
      {{.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
       {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  // Clear window
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  std::array<GLfloat, 2> sizes{};
#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
  abcg::glGetFloatv(GL_POINT_SIZE_RANGE, sizes.data());
#else
  abcg::glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, sizes.data());
#endif
  fmt::print("Point size: {:.2f} (min), {:.2f} (max)\n", sizes.at(0),
             sizes.at(1));

  // Start pseudorandom number generator
  auto const seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  // Randomly pick a pair of coordinates in the range [-1; 1)
  std::uniform_real_distribution<float> realDistribution(-1.0f, 1.0f);
  m_P.x = realDistribution(m_randomEngine);
  m_P.y = realDistribution(m_randomEngine);
}

void Window::onPaint() {
  // Create OpenGL buffers for drawing the point at m_P
  setupModel();

  // Set the viewport
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // Start using the shader program
  abcg::glUseProgram(m_program);
  // Start using VAO
  abcg::glBindVertexArray(m_VAO);

  // Draw a single point
  abcg::glDrawArrays(GL_POINTS, 0, 1);

  // End using VAO
  abcg::glBindVertexArray(0);
  // End using the shader program
  abcg::glUseProgram(0);

  // Randomly pick the index of a triangle vertex
  std::uniform_int_distribution<int> intDistribution(0, m_points.size() - 1);
  auto const index{intDistribution(m_randomEngine)};

  // The new position is the midpoint between the current position and the
  // chosen vertex position
  m_P = (m_P + m_points.at(index)) / 2.0f;

  // Print coordinates to console
  // fmt::print("({:+.2f}, {:+.2f})\n", m_P.x, m_P.y);
}

void Window::setupModel() {
  // Release previous VBO and VAO
  abcg::glDeleteBuffers(1, &m_VBOVertices);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Generate a new VBO and get the associated ID
  abcg::glGenBuffers(1, &m_VBOVertices);
  // Bind VBO in order to use it
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);
  // Upload data to VBO
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_P), &m_P, GL_STATIC_DRAW);
  // Unbinding the VBO is allowed (data can be released now)
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  // Release shader program, VBO and VAO
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOVertices);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    ImGui::SetNextWindowPos(ImVec2(5, 81));
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoDecoration);

    if (ImGui::Button("Clear window", ImVec2(150, 30))) {
      abcg::glClear(GL_COLOR_BUFFER_BIT);
    }

    ImGui::End();
  }
}

void drawCircle(glm::vec2 center, float radius, glm::vec4 color) {
  // Generate VBO with 32 vertices for a circle
  std::vector<glm::vec2> positions;
  for (int i = 0; i < 32; i++) {
    float angle = glm::radians(360.0f * i / 32.0f);
    positions.emplace_back(center + radius * glm::vec2{cos(angle), sin(angle)});
  }
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * positions.size(),
               positions.data(), GL_STATIC_DRAW);

  // Use the shader program
  glUseProgram(0);

  // Bind the VAO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Set the vertex attribute pointers
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Set the uniforms
  glColor4fv(&color[0]);

  // Draw the circle
  glDrawArrays(GL_TRIANGLE_FAN, 0, 32);

  // Unbind the VAO and VBO
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}