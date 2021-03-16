#include "ammobar.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void AmmoBar::initializeGL(GLuint program){
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  //m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  //m_scaleLoc = glGetUniformLocation(m_program, "scale");
  //m_translationLoc = glGetUniformLocation(m_program, "translation");

  //m_rotation = 0.0f;
  //m_translation = glm::vec2(0);
  //m_velocity = glm::vec2(0);

  std::array<glm::vec2, 4> positions{
     
      glm::vec2{-93.0f, -93.0f}, glm::vec2{-93.0f, +22.0f},
      glm::vec2{+96.0f, -92.0f}, glm::vec2{+96.0f, -93.0f},

     
  };

  //Normalize
  for (auto &position :positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  std::array indices{
                     0, 1, 2,
                     0, 2, 3,
                    };

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
}

void AmmoBar::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  glUseProgram(m_program);

  glBindVertexArray(m_vao);

 // glUniform1f(m_scaleLoc, m_scale);
//   glUniform1f(m_rotationLoc, m_rotation);
//   glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
 

  

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glDrawElements(GL_LINE_LOOP, 2 * 3, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);

  glUseProgram(0);
}

void AmmoBar::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}

void AmmoBar::update(const GameData &gameData)  {
  if (gameData.m_input[static_cast<size_t>(Input::Fire)])
  {
      //Lógica para decrementar as barrinhas de munições
  }
}