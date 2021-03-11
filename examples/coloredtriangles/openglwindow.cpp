#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "abcg.hpp"

void OpenGLWindow::initializeGL() {
  const auto *vertexShader{R"gl(
    #version 410

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    out vec4 fragColor;

    void main() {
      gl_Position = vec4(inPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};

  const auto *fragmentShader{R"gl(
    #version 410

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};

  // Create shader program
  m_program = createProgramFromString(vertexShader, fragmentShader);

  // Clear window
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);
}

void OpenGLWindow::paintGL() {
  if (m_timeElapsed >= m_renderDelay) {
    m_timeElapsed = 0;

    if (randomColors) {
      m_vertexColors[0] = getRandomVertexColor();
      m_vertexColors[1] = getRandomVertexColor();
      m_vertexColors[2] = getRandomVertexColor();
      m_flatColor = getRandomVertexColor();
    }

    setupModel();

    glViewport(0, 0, m_viewportWidth, m_viewportHeight);

    glUseProgram(m_program);
    glBindVertexArray(m_vao);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glUseProgram(0);
  }
}

glm::vec4 OpenGLWindow::getRandomVertexColor() {
  std::uniform_real_distribution<float> color_rd(0.0f, 1.0f);
  return glm::vec4{color_rd(m_randomEngine), color_rd(m_randomEngine),
                   color_rd(m_randomEngine), 1.0f};
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    auto widgetSize{ImVec2(450, 300)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5,
                                   m_viewportHeight - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);
    auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin(" ", nullptr, windowFlags);

    // ImGui::ShowDemoWindow();
    // Edit vertex colors
    auto colorEditFlags{ImGuiColorEditFlags_NoTooltip |
                        ImGuiColorEditFlags_NoPicker};
    ImGui::PushItemWidth(415);

    ImGui::Checkbox("Random Colors", &randomColors);
    ImGui::NewLine();
    ImGui::Checkbox("Solid Color", &flatColor);
    ImGui::NewLine();

    if (flatColor) {
      ImGui::ColorEdit3("", &m_flatColor.x, colorEditFlags);
      std::printf("%f", m_flatColor.x);
    } else {
      ImGui::PushItemWidth(240);
      ImGui::ColorEdit3("v0", &m_vertexColors[0].x, colorEditFlags);
      ImGui::ColorEdit3("v1", &m_vertexColors[1].x, colorEditFlags);
      ImGui::ColorEdit3("v2", &m_vertexColors[2].x, colorEditFlags);
      ImGui::PopItemWidth();
    }

    ImGui::SliderInt("Delay", &m_renderDelay, 0, 500);
    fmt::print("slider value = {}\n", m_renderDelay);
    ImGui::SameLine();
    ImGui::PopItemWidth();

    m_timeElapsed += 1000 / ImGui::GetIO().Framerate;

    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_vboPositions);
  glDeleteBuffers(1, &m_vboColors);
  glDeleteVertexArrays(1, &m_vao);
}

void OpenGLWindow::setupModel() {
  glDeleteBuffers(1, &m_vboPositions);
  glDeleteBuffers(1, &m_vboColors);
  glDeleteVertexArrays(1, &m_vao);

  // Create vertex positions
  std::uniform_real_distribution<float> rd(-1.5f, 1.5f);
  std::array positions{glm::vec2(rd(m_randomEngine), rd(m_randomEngine)),
                       glm::vec2(rd(m_randomEngine), rd(m_randomEngine)),
                       glm::vec2(rd(m_randomEngine), rd(m_randomEngine))};

  // Create vertex colors
  std::vector<glm::vec4> colors(0);
  if (flatColor) {
    colors.push_back(m_flatColor);
    colors.push_back(m_flatColor);
    colors.push_back(m_flatColor);
  } else {
    colors.push_back(m_vertexColors[0]);
    colors.push_back(m_vertexColors[1]);
    colors.push_back(m_vertexColors[2]);
  }
  // Generate VBO of positions
  glGenBuffers(1, &m_vboPositions);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of colors
  glGenBuffers(1, &m_vboColors);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4),
               colors.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute = glGetAttribLocation(m_program, "inPosition");
  GLint colorAttribute = glGetAttribLocation(m_program, "inColor");

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glEnableVertexAttribArray(colorAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);
}