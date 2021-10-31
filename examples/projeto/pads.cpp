#include "pads.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Pads::initializeGL(GLuint program, glm::vec2 startPosition, int side) {
  terminateGL();

  // Start pseudo-random number generator
  // m_randomEngine.seed(
  //     std::chrono::steady_clock::now().time_since_epoch().count());

  m_side = side;
  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_padPosition = startPosition;
  m_color = glm::vec4(1);
  m_color.a = 1.0f;

  // Create geometry                                         // modificar isso, precisamos de uma geometria fixa!
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(m_padPosition.x -0.05f, m_padPosition.y -0.2f);
  positions.emplace_back(m_padPosition.x -0.05f, m_padPosition.y +0.2f);
  positions.emplace_back(m_padPosition.x +0.05f, m_padPosition.y +0.2f);
  positions.emplace_back(m_padPosition.x +0.05f, m_padPosition.y -0.2f);
  positions.push_back(positions.at(0));

  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
  // m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  // m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  // m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create pads
  // m_pad.clear();
  // m_pad.resize(2);
  
  // glm::vec2 left_position{-0.5f,0.0f};
  // glm::vec2 right_position{0.5f,0.0f};
  // m_pads.front().m_translation = left_position;
  // m_pads.back().m_translation = right_position;
  // }
}

void Pads::paintGL() {
  abcg::glUseProgram(m_program);

  // Create geometry                                         // modificar isso, precisamos de uma geometria fixa!
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(m_padPosition.x -0.02f, m_padPosition.y -0.18f);
  positions.emplace_back(m_padPosition.x -0.02f, m_padPosition.y +0.18f);
  positions.emplace_back(m_padPosition.x +0.02f, m_padPosition.y +0.18f);
  positions.emplace_back(m_padPosition.x +0.02f, m_padPosition.y -0.18f);
  positions.push_back(positions.at(0));

  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  abcg::glBindVertexArray(m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  // abcg::glUniform1f(m_scaleLoc, pad.m_scale);
  // abcg::glUniform1f(m_rotationLoc, pad.m_rotation);

  
  // abcg::glUniform2f(m_translationLoc, pad.m_translation.x + j,
                        // pad.m_translation.y + i);

  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  
  abcg::glBindVertexArray(0);
  
  abcg::glUseProgram(0);
}

void Pads::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Pads::update(GameData gamedata, float deltaTime) {    // no caso dos pads, nao precisamos receber nada, só a bolinha vai ter que receber as pads!
  
  glm::vec2 move{0.0f,0.5f};

  if(gamedata.m_input[static_cast<size_t>(Input::LUp)] && m_side == 1){
    m_padPosition += move * deltaTime;
  }
  if(gamedata.m_input[static_cast<size_t>(Input::LDown)] && m_side == 1){
    m_padPosition -= move * deltaTime;
  }
  if(gamedata.m_input[static_cast<size_t>(Input::RUp)] && m_side == 2){
    m_padPosition += move * deltaTime;
  }
  if(gamedata.m_input[static_cast<size_t>(Input::RDown)] && m_side == 2){
    m_padPosition -= move * deltaTime;
  }

  //   // Wrap-around
  //   if (pad.m_translation.x < -1.0f) pad.m_translation.x += 2.0f;
  //   if (pad.m_translation.x > +1.0f) pad.m_translation.x -= 2.0f;
  //   if (pad.m_translation.y < -1.0f) pad.m_translation.y += 2.0f;
  //   if (pad.m_translation.y > +1.0f) pad.m_translation.y -= 2.0f;
  // }
}