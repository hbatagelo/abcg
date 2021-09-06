#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <gsl/gsl>

#include "abcg.hpp"

void OpenGLWindow::initializeGL() {
  // Enable Z-buffer test
  glEnable(GL_DEPTH_TEST);

  // Create shader program
  m_program = createProgramFromFile(getAssetsPath() + "UnlitVertexColor.vert",
                                    getAssetsPath() + "UnlitVertexColor.frag");

  // clang-format off
  std::array vertices{glm::vec2(0.0f, 0.5f), 
                      glm::vec2(0.5f, -0.5f),
                      glm::vec2(-0.5f, -0.5f)};
  std::array colors{glm::vec3(1.0f, 0.0f, 0.0f), 
                    glm::vec3(1.0f, 0.0f, 1.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)};
  // clang-format on

  // Generate a new VBO and get the associated ID
  glGenBuffers(1, &m_vboVertices);
  // Bind VBO in order to use it
  glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
  // Upload data to VBO
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
               GL_STATIC_DRAW);
  // Unbinding the VBO is allowed (data can be released now)
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &m_vboColors);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
  GLint colorAttribute{glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glEnableVertexAttribArray(colorAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  glClearColor(gsl::at(m_clearColor, 0), gsl::at(m_clearColor, 1),
               gsl::at(m_clearColor, 2), gsl::at(m_clearColor, 3));
  // Clear the color buffer and Z-buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Adjust viewport
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // Start using the shader program
  glUseProgram(m_program);
  // Start using the VAO
  glBindVertexArray(m_vao);

  // Render a nice colored triangle
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // End using the VAO
  glBindVertexArray(0);
  // End using the shader program
  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::OpenGLWindow::paintUI();

  // Our own ImGui widgets go below
  {
    // If this is the first frame, set initial position of our window
    static bool firstTime{true};
    if (firstTime) {
      ImGui::SetNextWindowPos(ImVec2(5, 75));
      firstTime = false;
    }

    // Window begin
    ImGui::Begin("Hello! This is a Dear ImGui window");

    // Static text
    ImGui::Text("Some example widgets are given below.");

    // Combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"First item", "Second item",
                                          "Third item", "Fourth item"};

      if (ImGui::BeginCombo("A combo box",
                            comboItems.at(currentIndex).c_str())) {
        for (auto index : iter::range(comboItems.size())) {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;

          // Set the initial focus when opening the combo (scrolling + keyboard
          // navigation focus)
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
    }

    // Edit bools storing our window open/close state
    ImGui::Checkbox("Show demo window", &m_showDemoWindow);
    ImGui::Checkbox("Show another window", &m_showAnotherWindow);

    // Slider from 0.0f to 1.0f
    static float f{};
    ImGui::SliderFloat("A slider", &f, 0.0f, 1.0f);

    // Edit background color
    ImGui::ColorEdit3("Background", m_clearColor.data());

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // End of window
    ImGui::End();

    // Show the big demo window (Most of the sample code is in
    // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
    // ImGui!).
    if (m_showDemoWindow) {
      ImGui::ShowDemoWindow(&m_showDemoWindow);
    }

    // Show another simple window
    if (m_showAnotherWindow) {
      // Pass a pointer to our bool variable (the window will have a closing
      // button that will clear the bool when clicked)
      ImGui::Begin("Another window", &m_showAnotherWindow);
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me")) m_showAnotherWindow = false;
      ImGui::End();
    }
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
}

void OpenGLWindow::terminateGL() {
  // Release OpenGL resources
  glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_vboVertices);
  glDeleteBuffers(1, &m_vboColors);
  glDeleteVertexArrays(1, &m_vao);
}
