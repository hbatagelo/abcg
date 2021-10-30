#include "pacmans.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include "ghost.hpp"

void Pacmans::generatePacmans() {
  for (auto &pacman : m_pacmans) {
    pacman = createPacman();
    pacman.m_translation = {m_randomDist(m_randomEngine), 1.5f};
  }
}

void Pacmans::initializeGL(GLuint program, int quantity) {
  terminateGL();

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_pacmans.clear();
  m_pacmans.resize(quantity);
  generatePacmans();
}

void Pacmans::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  glUseProgram(m_program);

  // Wait 5 seconds before restarting
  if (m_generatetWaitTimer.elapsed() > 3) {
    m_generatetWaitTimer.restart();
    generatePacmans();
  }

  for (auto &pacman : m_pacmans) {
    glBindVertexArray(pacman.m_vao);

    glUniform4fv(m_colorLoc, 1, &pacman.m_color.r);
    glUniform1f(m_scaleLoc, pacman.m_scale);

    glUniform2f(m_translationLoc, pacman.m_translation.x,
                pacman.m_translation.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, pacman.m_polygonSides);

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Pacmans::terminateGL() {
  for (auto pacman : m_pacmans) {
    glDeleteBuffers(1, &pacman.m_vbo);
    glDeleteVertexArrays(1, &pacman.m_vao);
  }
}

void Pacmans::update(float deltaTime) {
  for (auto &pacman : m_pacmans) {
    pacman.m_translation += pacman.m_velocity * deltaTime;
  }
}

Pacmans::Pacman Pacmans::createPacman(glm::vec2 translation, float scale) {
  Pacman pacman;

  // auto &re{m_randomEngine};
  pacman.m_polygonSides = 10;

  // Choose color (actually yellow)
  pacman.m_color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
  pacman.m_scale = scale;
  pacman.m_translation = translation;

  // Choose a random angular velocity
  // pacman.m_angularVelocity = m_randomDist(re);

  // Choose a direction
  glm::vec2 direction{0.0f, -1.0f};
  pacman.m_velocity = glm::normalize(direction);

  // Create Pacman Geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto step{M_PI * 2 / pacman.m_polygonSides};
  for (auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  glGenBuffers(1, &pacman.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, pacman.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &pacman.m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(pacman.m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, pacman.m_vbo);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);

  return pacman;
}
