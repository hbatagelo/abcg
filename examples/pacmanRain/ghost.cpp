#include "ghost.hpp"

void Ghost::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_translation = glm::vec2(0, -0.5);
  m_velocity = glm::vec2(0);

  std::array<glm::vec2, 17> positions{
      // Head
      glm::vec2{-2.0f, -4.0f},
      glm::vec2{-2.0f, -6.0f},
      glm::vec2{2.0f, -4.0f},
      glm::vec2{2.0f, -6.0f},

      // Body
      glm::vec2{-4.0f, -12.0f},
      glm::vec2{4.0f, -12.0f},
      glm::vec2{-4.0f, -6.0f},
      glm::vec2{4.0f, -6.0f},

      // Foot
      glm::vec2{-4.0f, -13.0f},
      glm::vec2{-3.0f, -12.0f},
      glm::vec2{-2.0f, -13.0f},
      glm::vec2{-1.0f, -12.0f},
      glm::vec2{0.0f, -13.0f},
      glm::vec2{1.0f, -12.0f},
      glm::vec2{2.0f, -13.0f},
      glm::vec2{3.0f, -12.0f},
      glm::vec2{4.0f, -13.0f},
  };

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  const std::array indices{0,  6,  1,  0,  1,  2,  2,  1,  3,  2,  3,
                           7,  4,  5,  6,  5,  6,  7,  4,  8,  9,  9,
                           10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 5};

  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Ghost::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_vao);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 11 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Ghost::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteBuffers(1, &m_ebo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Ghost::update(const GameData &gameData) {
  if (m_ghostTimer.elapsed() > 10.0 / 1000.0) {
    m_ghostTimer.restart();

    if (gameData.m_input[static_cast<size_t>(Input::Left)]) {
      if (m_translation.x - 0.02f >= -1.0f) m_translation.x -= 0.02f;
      if (m_translation.x - 0.12f < -1.0f) m_translation.x = -0.88f;
    }

    if (gameData.m_input[static_cast<size_t>(Input::Right)]) {
      if (m_translation.x + 0.02f <= +1.0f) m_translation.x += 0.02f;
      if (m_translation.x + 0.12f > 1.0f) m_translation.x = 0.88f;
    }
  }
}