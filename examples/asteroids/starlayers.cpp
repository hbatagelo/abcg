#include "starlayers.hpp"

#include <cppitertools/itertools.hpp>

void StarLayers::initializeGL(GLuint program, int quantity) {
  terminateGL();

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  m_program = program;
  m_pointSizeLoc = glGetUniformLocation(m_program, "pointSize");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  auto &re{m_randomEngine};
  std::uniform_real_distribution<float> distPos(-1.0f, 1.0f);
  std::uniform_real_distribution<float> distIntensity(0.5f, 1.0f);

  for (auto &&[index, layer] : iter::enumerate(m_starLayers)) {
    layer.m_pointSize = 10.0f / (1.0f + index);
    layer.m_quantity = quantity * (static_cast<int>(index) + 1);
    layer.m_translation = glm::vec2(0);

    std::vector<glm::vec3> data(0);
    for ([[maybe_unused]] auto i : iter::range(0, layer.m_quantity)) {
      data.emplace_back(distPos(re), distPos(re), 0);
      data.push_back(glm::vec3(1) * distIntensity(re));
    }

    // Generate VBO
    glGenBuffers(1, &layer.m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, layer.m_vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), data.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Get location of attributes in the program
    GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
    GLint colorAttribute{glGetAttribLocation(m_program, "inColor")};

    // Create VAO
    glGenVertexArrays(1, &layer.m_vao);

    // Bind vertex attributes to current VAO
    glBindVertexArray(layer.m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, layer.m_vbo);
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3) * 2, nullptr);
    glEnableVertexAttribArray(colorAttribute);
    glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3) * 2,
                          reinterpret_cast<void *>(sizeof(glm::vec3)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // End of binding to current VAO
    glBindVertexArray(0);
  }
}

void StarLayers::paintGL() {
  glUseProgram(m_program);

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);

  for (auto &layer : m_starLayers) {
    glBindVertexArray(layer.m_vao);
    glUniform1f(m_pointSizeLoc, layer.m_pointSize);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        glUniform2f(m_translationLoc, layer.m_translation.x + j,
                    layer.m_translation.y + i);

        glDrawArrays(GL_POINTS, 0, layer.m_quantity);
      }
    }

    glBindVertexArray(0);
  }

  glDisable(GL_BLEND);

  glUseProgram(0);
}

void StarLayers::terminateGL() {
  for (auto &layer : m_starLayers) {
    glDeleteBuffers(1, &layer.m_vbo);
    glDeleteVertexArrays(1, &layer.m_vao);
  }
}

void StarLayers::update(const Ship &ship, float deltaTime) {
  for (auto &&[index, layer] : iter::enumerate(m_starLayers)) {
    auto layerSpeedScale{1.0f / (index + 2.0f)};
    layer.m_translation -= ship.m_velocity * deltaTime * layerSpeedScale;

    // Wrap-around
    if (layer.m_translation.x < -1.0f) layer.m_translation.x += 2.0f;
    if (layer.m_translation.x > +1.0f) layer.m_translation.x -= 2.0f;
    if (layer.m_translation.y < -1.0f) layer.m_translation.y += 2.0f;
    if (layer.m_translation.y > +1.0f) layer.m_translation.y -= 2.0f;
  }
}