#include "window.hpp"

void Window::onCreate() {
  // Enable Z-buffer test
  abcg::glEnable(GL_DEPTH_TEST);

  // Create shader program
  auto const path{abcg::Application::getAssetsPath()};
  m_program =
      abcg::createOpenGLProgram({{.source = path + "UnlitVertexColor.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = path + "UnlitVertexColor.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // clang-format off
  std::array const vertices{glm::vec2(0.0f, 0.5f),
                            glm::vec2(0.5f, -0.5f),
                            glm::vec2(-0.5f, -0.5f)};
  std::array const colors{glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 
                          glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
                          glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)};
  // clang-format on

  // Generate a new VBO and get the associated ID
  abcg::glGenBuffers(1, &m_vboVertices);
  // Bind VBO in order to use it
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
  // Upload data to VBO
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
                     GL_STATIC_DRAW);
  // Unbinding the VBO is allowed (data can be released now)
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glGenBuffers(1, &m_vboColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  auto const colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::onPaint() {
  // Set the clear color
  abcg::glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b,
                     m_clearColor.a);
  // Clear the color buffer and Z-buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Adjust viewport
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // Start using the shader program
  abcg::glUseProgram(m_program);
  // Start using the VAO
  abcg::glBindVertexArray(m_vao);

  // Render a triangle
  abcg::glDrawArrays(GL_TRIANGLES, 0, 3);

  // End using the VAO
  abcg::glBindVertexArray(0);
  // End using the shader program
  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::OpenGLWindow::onPaintUI();

  // Our own ImGui widgets go below
  {
    // If this is the first frame, set initial position of our window
    static auto firstTime{true};
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
      static auto currentIndex{0U};
      std::vector<std::string> const comboItems{"First item", "Second item",
                                                "Third item", "Fourth item"};

      if (ImGui::BeginCombo("A combo box",
                            comboItems.at(currentIndex).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          bool const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;

          // Set the initial focus when opening the combo (scrolling + keyboard
          // navigation focus)
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
    }

    // Edit bools storing our window open/close state
    ImGui::Checkbox("Show demo window", &m_showDemoWindow);
    ImGui::Checkbox("Show another window", &m_showAnotherWindow);

    // Slider from 0.0f to 1.0f
    static auto value{0.0f};
    ImGui::SliderFloat("A slider", &value, 0.0f, 1.0f);

    // Edit background color
    ImGui::ColorEdit3("Background", &m_clearColor.r);

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
      if (ImGui::Button("Close Me"))
        m_showAnotherWindow = false;
      ImGui::End();
    }
  }
}

void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::onDestroy() {
  // Release OpenGL resources
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_vboVertices);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);
}
