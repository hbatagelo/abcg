#include "asteroids.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Asteroids::initializeGL(GLuint program, int quantity) {
  terminateGL();

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  // Create asteroids
  m_asteroids.clear();
  m_asteroids.resize(quantity);

  for (auto &asteroid : m_asteroids) {
    asteroid = createAsteroid();

    // Make sure the asteroid won't collide with the ship
    do {
      asteroid.m_translation = {m_randomDist(m_randomEngine),
                                m_randomDist(m_randomEngine)};
    } while (glm::length(asteroid.m_translation) < 0.5f);
  }
}

void Asteroids::paintGL() {
  glUseProgram(m_program);

  for (auto &asteroid : m_asteroids) {
    glBindVertexArray(asteroid.m_vao);

    glUniform4fv(m_colorLoc, 1, &asteroid.m_color.r);
    glUniform1f(m_scaleLoc, asteroid.m_scale);
    glUniform1f(m_rotationLoc, asteroid.m_rotation);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        glUniform2f(m_translationLoc, asteroid.m_translation.x + j,
                    asteroid.m_translation.y + i);

        glDrawArrays(GL_TRIANGLE_FAN, 0, asteroid.m_polygonSides + 2);
      }
    }

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Asteroids::terminateGL() {
  for (auto asteroid : m_asteroids) {
    glDeleteBuffers(1, &asteroid.m_vbo);
    glDeleteVertexArrays(1, &asteroid.m_vao);
  }
}

void Asteroids::update(const Ship &ship, float deltaTime) {
  for (auto &asteroid : m_asteroids) {
    asteroid.m_translation -= ship.m_velocity * deltaTime;
    asteroid.m_rotation = glm::wrapAngle(
        asteroid.m_rotation + asteroid.m_angularVelocity * deltaTime);
    asteroid.m_translation += asteroid.m_velocity * deltaTime;

    // Wrap-around
    if (asteroid.m_translation.x < -1.0f) asteroid.m_translation.x += 2.0f;
    if (asteroid.m_translation.x > +1.0f) asteroid.m_translation.x -= 2.0f;
    if (asteroid.m_translation.y < -1.0f) asteroid.m_translation.y += 2.0f;
    if (asteroid.m_translation.y > +1.0f) asteroid.m_translation.y -= 2.0f;
  }
}

Asteroids::Asteroid Asteroids::createAsteroid(glm::vec2 translation,
                                              float scale) {
  Asteroid asteroid;

  auto &re{m_randomEngine};  // Shortcut

  // Randomly choose the number of sides
  std::uniform_int_distribution<int> randomSides(6, 20);
  asteroid.m_polygonSides = randomSides(re);

  // Choose a random color (actually, a grayscale)
  std::uniform_real_distribution<float> randomIntensity(0.5f, 1.0f);
  asteroid.m_color = glm::vec4(1) * randomIntensity(re);

  asteroid.m_color.a = 1.0f;
  asteroid.m_rotation = 0.0f;
  asteroid.m_scale = scale;
  asteroid.m_translation = translation;

  // Choose a random angular velocity
  asteroid.m_angularVelocity = m_randomDist(re);

  // Choose a random direction
  glm::vec2 direction{m_randomDist(re), m_randomDist(re)};
  asteroid.m_velocity = glm::normalize(direction) / 7.0f;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto step{M_PI * 2 / asteroid.m_polygonSides};
  std::uniform_real_distribution<float> randomRadius(0.8f, 1.0f);
  for (auto angle : iter::range(0.0, M_PI * 2, step)) {
    auto radius{randomRadius(re)};
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  glGenBuffers(1, &asteroid.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, asteroid.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &asteroid.m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(asteroid.m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, asteroid.m_vbo);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);

  return asteroid;
}