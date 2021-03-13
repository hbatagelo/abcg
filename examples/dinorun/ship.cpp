#include "ship.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

glm::vec4 Ship::getRandomVertexColor() {
  std::uniform_real_distribution<float> color_rd(0.0f, 1.0f);
  return glm::vec4{color_rd(m_randomEngine), color_rd(m_randomEngine),
                   color_rd(m_randomEngine), 1.0f};
}

void Ship::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  std::array<glm::vec2, 16> positions{
      // Ship body
      glm::vec2{-15.0f, +30.0f}, glm::vec2{+20.0f, +30.0f},
      glm::vec2{+20.0f, +12.5f},

      glm::vec2{-15.0f, +12.5f},

      glm::vec2{-7.0f, +12.5f},

      glm::vec2{-7.0f, -12.5f},

      glm::vec2{-44.0f, -12.5f}, glm::vec2{-7.0f, +5.5f},
      glm::vec2{-7.0f, +1.5f},   glm::vec2{0.0f, +1.5f},

      glm::vec2{-15.0f, -12.5f}, glm::vec2{-15.0f, -20.0f},
      glm::vec2{-6.0f, -20.0f},

      glm::vec2{-30.0f, -12.5f}, glm::vec2{-30.0f, -20.0f},
      glm::vec2{-21.0f, -20.0f},

  };

  m_color = getRandomVertexColor();

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  std::array indices{0, 1, 2, 0,  2,  3,  4,  5,  6,
                     7, 9, 8, 10, 11, 12, 13, 14, 15};

  // Generate VBO
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  glBindVertexArray(0);

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);
}

void Ship::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  glUseProgram(m_program);

  glBindVertexArray(m_vao);

  glUniform1f(m_scaleLoc, m_scale);
  glUniform1f(m_rotationLoc, m_rotation);
  glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) m_trailBlinkTimer.restart();

  if (gameData.m_input[static_cast<size_t>(Input::Up)]) {
    // Show thruster trail during 50 ms
    if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // 50% transparent
      glUniform4f(m_colorLoc, 1, 1, 1, 0.5f);

      glDrawElements(GL_TRIANGLES, 14 * 3, GL_UNSIGNED_INT, nullptr);

      glDisable(GL_BLEND);
    }
  }

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);

  glUseProgram(0);
}

void Ship::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}

void Ship::update(const GameData &gameData, float deltaTime) {
  /*Rotate
 if (gameData.m_input[static_cast<size_t>(Input::Left)])
   m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);
 if (gameData.m_input[static_cast<size_t>(Input::Right)])
   m_rotation = glm::wrapAngle(m_rotation - 4.0f * deltaTime); */

  /*Apply thrust
  if (gameData.m_input[static_cast<size_t>(Input::Up)] &&
      gameData.m_state == State::Playing) {
    // Thrust in the forward vector
    glm::vec2 forward = glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation);
    forward = glm::rotate(glm::vec2{0.0f, -1.0f}, m_rotation);
    m_velocity += forward * deltaTime;
  }*/

  if (gameData.m_input[static_cast<size_t>(Input::Fire)] &&
      gameData.m_state == State::Playing) {
    m_velocity.x = 1.0f;
    m_velocity.y = 0.0f;
  }

  // if (gameData.m_input[static_cast<size_t>(Input::Left)] &&
  // gameData.m_state == State::Playing) {
  // glm::vec2 forward = glm::rotate(glm::vec2{-1.0f, 0.0f}, m_rotation);
  // m_velocity += forward * deltaTime;
  // }

  // if (gameData.m_input[static_cast<size_t>(Input::Right)] &&
  // gameData.m_state == State::Playing) {
  // glm::vec2 forward = glm::rotate(glm::vec2{1.0f, 0.0f}, m_rotation);
  // m_velocity += forward * deltaTime;
  // }

  // if (gameData.m_input[static_cast<size_t>(Input::Down)] &&
  // gameData.m_state == State::Playing) {
  // glm::vec2 forward = glm::rotate(glm::vec2{0.0f, -1.0f}, m_rotation);
  // m_velocity += forward * deltaTime;
  // }
}