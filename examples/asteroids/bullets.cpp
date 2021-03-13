#include "bullets.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Bullets::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_bullets.clear();

  // Create regular polygon
  auto sides{10};

  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto step{M_PI * 2 / sides};
  for (auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO of positions
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

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

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Bullets::paintGL() {
  glUseProgram(m_program);

  glBindVertexArray(m_vao);
  glUniform4f(m_colorLoc, 1, 1, 1, 1);
  glUniform1f(m_rotationLoc, 0);
  glUniform1f(m_scaleLoc, m_scale);

  for (auto &bullet : m_bullets) {
    glUniform2f(m_translationLoc, bullet.m_translation.x,
                bullet.m_translation.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
  }

  glBindVertexArray(0);

  glUseProgram(0);
}

void Bullets::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Bullets::update(Ship &ship, const GameData &gameData, float deltaTime) {
  // Create a pair of bullets
  if (gameData.m_input[static_cast<size_t>(Input::Fire)] &&
      gameData.m_state == State::Playing) {
    // At least 250 ms must have passed since the last bullets
    if (ship.m_bulletCoolDownTimer.elapsed() > 250.0 / 1000.0) {
      ship.m_bulletCoolDownTimer.restart();

      // Bullets are shot in the direction of the ship's forward vector
      glm::vec2 forward{glm::rotate(glm::vec2{0.0f, 1.0f}, ship.m_rotation)};
      glm::vec2 right{glm::rotate(glm::vec2{1.0f, 0.0f}, ship.m_rotation)};
      auto cannonOffset{(11.0f / 15.5f) * ship.m_scale};
      auto bulletSpeed{2.0f};

      Bullet bullet{.m_dead = false,
                    .m_translation = ship.m_translation + right * cannonOffset,
                    .m_velocity = ship.m_velocity + forward * bulletSpeed};
      m_bullets.push_back(bullet);

      bullet.m_translation = ship.m_translation - right * cannonOffset;
      m_bullets.push_back(bullet);

      // Moves ship in the opposite direction
      ship.m_velocity -= forward * 0.1f;
    }
  }

  for (auto &bullet : m_bullets) {
    bullet.m_translation -= ship.m_velocity * deltaTime;
    bullet.m_translation += bullet.m_velocity * deltaTime;

    // Kill bullet if it goes off screen
    if (bullet.m_translation.x < -1.1f) bullet.m_dead = true;
    if (bullet.m_translation.x > +1.1f) bullet.m_dead = true;
    if (bullet.m_translation.y < -1.1f) bullet.m_dead = true;
    if (bullet.m_translation.y > +1.1f) bullet.m_dead = true;
  }

  // Remove dead bullets
  m_bullets.remove_if([](const Bullet &p) { return p.m_dead; });
}