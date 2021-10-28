#include "ball.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Ball::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  const auto ballSpeed{1.0f};
  m_dead = false,
  m_translation = glm::vec2{0.0f, 0.0f},
  m_velocity = glm::vec2{1.0f, 0.0f} * ballSpeed;

  // Create regular polygon
  const auto sides{10};

  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  const auto step{M_PI * 2 / sides};
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const GLint positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Ball::paintGL() {
  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_vao);
  abcg::glUniform4f(m_colorLoc, 1, 1, 1, 1);
  abcg::glUniform1f(m_rotationLoc, 0);
  abcg::glUniform1f(m_scaleLoc, m_scale);


  abcg::glUniform2f(m_translationLoc, m_translation.x, m_translation.y);

  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 12);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Ball::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Ball::update(BarLeft &barLeft, BarRight &barRight,const GameData &gameData, float deltaTime) {
  // Create Ball
  if (!direction) {
    m_translation -= m_velocity * deltaTime;
  }else{
    m_translation += m_velocity * deltaTime;
  }
  

  // ball.m_translation = barLeft.m_translation - right * cannonOffset;
  // ball.m_translation -= ball.m_velocity * deltaTime

  /*
  if (gameData.m_input[static_cast<size_t>(Input::Fire)] && gameData.m_state == State::Playing) {
    // At least 250 ms must have passed since the last Ball
    if (barLeft.m_ballCoolDownTimer.elapsed() > 250.0 / 1000.0) {
      barLeft.m_ballCoolDownTimer.restart();

      // balls are shot in the direction of the barLeft's forward vector
      glm::vec2 forward{glm::rotate(glm::vec2{1.0f, 0.0f}, barLeft.m_rotation)};
      glm::vec2 right{glm::rotate(glm::vec2{1.0f, 0.0f}, barLeft.m_rotation)};
      const auto cannonOffset{(11.0f / 15.5f) * barLeft.m_scale};
      const auto ballSpeed{2.0f};

      Ball ball{.m_dead = false,
                .m_translation = barLeft.m_translation + right * cannonOffset,
                .m_velocity = barLeft.m_velocity + forward * ballSpeed};
      m_balls.push_back(ball);

      ball.m_translation = barLeft.m_translation - right * cannonOffset;
      m_balls.push_back(ball);

      // Moves barLeft in the opposite direction
      // barLeft.m_velocity -= forward * 0.1f;
    }
  }


  for (auto &ball : m_balls) {
    ball.m_translation += glm::vec2{1.0f, 0.0f} * deltaTime;
    // ball.m_translation += ball.m_velocity * deltaTime;

    // Kill ball if it goes off screen
    if (ball.m_translation.x < -1.1f) ball.m_dead = true;
    if (ball.m_translation.x > +1.1f) ball.m_dead = true;
    if (ball.m_translation.y < -1.1f) ball.m_dead = true;
    if (ball.m_translation.y > +1.1f) ball.m_dead = true;
  }

  // Remove dead balls
  m_balls.remove_if([](const Ball &p) { return p.m_dead; });

  */
}
