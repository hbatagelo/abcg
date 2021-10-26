#include "squares.hpp"
#include "triangle.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Squares::initializeGL(GLuint program, int quantity) {
  terminateGL();

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_squares.clear();
  m_squares.resize(quantity);

  for (auto &square : m_squares) {
    square = createSquare();

    do {
      square.m_translation = {m_randomDist(m_randomEngine),
                                m_randomDist(m_randomEngine)};
    } while (glm::length(square.m_translation) < 0.5f);
  }
}

void Squares::paintGL() {
  glUseProgram(m_program);

  for (auto &square : m_squares) {
    glBindVertexArray(square.m_vao);

    glUniform4fv(m_colorLoc, 1, &square.m_color.r);
    glUniform1f(m_scaleLoc, square.m_scale);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        glUniform2f(m_translationLoc, square.m_translation.x + j,
                    square.m_translation.y + i);

        glDrawArrays(GL_TRIANGLE_FAN, 0, square.m_polygonSides);
      }
    }

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Squares::terminateGL() {
  for (auto square : m_squares) {
    glDeleteBuffers(1, &square.m_vbo);
    glDeleteVertexArrays(1, &square.m_vao);
  }
}

void Squares::update(const Triangle &triangle, float deltaTime) {
  for (auto &square : m_squares) {
    square.m_translation -= triangle.m_velocity * deltaTime;
    square.m_translation += square.m_velocity * deltaTime;

    // Wrap-around
    if (square.m_translation.x < -1.0f) square.m_translation.x += 2.0f;
    if (square.m_translation.x > +1.0f) square.m_translation.x -= 2.0f;
    if (square.m_translation.y < -1.0f) square.m_translation.y += 2.0f;
    if (square.m_translation.y > +1.0f) square.m_translation.y -= 2.0f;
  }
}

Squares::Square Squares::createSquare(glm::vec2 translation,
                                              float scale) {
  Square square;

  auto &re{m_randomEngine};
  square.m_polygonSides = 10;

  // Choose a random color (actually, a grayscale)
  square.m_color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
  square.m_scale = scale;
  square.m_translation = translation;

  // Choose a random angular velocity
  square.m_angularVelocity = m_randomDist(re);

  // Choose a random direction
  glm::vec2 direction{m_randomDist(re), m_randomDist(re)};
  square.m_velocity = glm::normalize(direction) / 5.0f;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto step{M_PI * 2 / square.m_polygonSides};
  for (auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  glGenBuffers(1, &square.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, square.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &square.m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(square.m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, square.m_vbo);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);

  return square;
}
