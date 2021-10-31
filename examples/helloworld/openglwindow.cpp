#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <gsl/gsl>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP)
      m_gameData.m_input.set(static_cast<size_t>(Input::RUp));
    if (event.key.keysym.sym == SDLK_DOWN)
      m_gameData.m_input.set(static_cast<size_t>(Input::RDown));
    if (event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.set(static_cast<size_t>(Input::LUp));
    if (event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.set(static_cast<size_t>(Input::LDown));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_UP)
      m_gameData.m_input.reset(static_cast<size_t>(Input::RUp));
    if (event.key.keysym.sym == SDLK_DOWN)
      m_gameData.m_input.reset(static_cast<size_t>(Input::RDown));
    if (event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.reset(static_cast<size_t>(Input::LUp));
    if (event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.reset(static_cast<size_t>(Input::LDown));
  }
}

void OpenGLWindow::initializeGL() {

  m_randomEngine.seed(std::chrono::steady_clock::now().time_since_epoch().count());

  std::uniform_real_distribution<float> m_randomDist{-0.5f, 0.5f};

  ball_xSpeed = m_randomDist(m_randomEngine);                  // Define a velocidade aleatória inicial em X da bolinha
  ball_ySpeed = m_randomDist(m_randomEngine);                  // Define a velocidade aleatória inicial em Y da bolinha

  // pad_leftSpeed = m_randomDist(m_randomEngine);                // Define a velocidade aleatória inicial da pad esquerda
  // pad_rightSpeed = m_randomDist(m_randomEngine);               // Define a velocidade aleatória inicial da pad direita

  pad_leftSpeed = 0.0f;                // Define a velocidade aleatória inicial da pad esquerda
  pad_rightSpeed = 0.0f;               // Define a velocidade aleatória inicial da pad direita

  // Enable Z-buffer test
  glEnable(GL_DEPTH_TEST);

  // Create shader program    mudar o shader para usar o dshader do asteroids!
  m_program = createProgramFromFile(getAssetsPath() + "objects.vert",getAssetsPath() + "objects.frag");


  glm::vec4 m_color{1};


  // Start using the shader program
  glUseProgram(m_program);
  // Start using the VAO
  glBindVertexArray(m_vao);
  
  // Pega os endereços das variáveis uniformes do shader
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // seta a translacao inicial em 0x0
  glUniform2f(m_translationLoc, 0, 0);

  // seta a cor do shader
  glUniform4f(m_colorLoc, 1, 1, 1, 0.5f);

  // seta a escala do objeto no shader
  glUniform1f(m_scaleLoc, 1);

  glUniform1f(m_rotationLoc, 0.0f);

  // clang-format off
  std::array vertices{glm::vec2(1.0f, 1.0f), glm::vec2(-1.0f, 1.0f),glm::vec2(1.0f, 0.95f), glm::vec2(-1.0f, 0.95f),              // parede de cima
                      glm::vec2(1.0f, -1.0f), glm::vec2(-1.0f, -1.0f),glm::vec2(1.0f, -0.95f), glm::vec2(-1.0f, -0.95f),           // parede de baixo
                      glm::vec2(-1.0f, (-0.2f + pad_leftCenter)), glm::vec2(-1.0f, (0.2f + pad_leftCenter)),glm::vec2(-0.975f, (-0.2f + pad_leftCenter)), glm::vec2(-0.975f, (0.2f + pad_leftCenter)),              // raquete esquerda
                      glm::vec2(1.0f, (-0.2f + pad_rightCenter)), glm::vec2(1.0f, (0.2f + pad_rightCenter)),glm::vec2(0.975f, (-0.2f + pad_rightCenter)), glm::vec2(0.975f, (0.2f + pad_rightCenter)),
                      glm::vec2((-0.025f + ball_xCenter), (ratio*(-0.025f + ball_yCenter))), glm::vec2((-0.025f + ball_xCenter), (ratio*(0.025f + ball_yCenter))),glm::vec2((0.025f + ball_xCenter), (ratio*(-0.025f + ball_yCenter))), glm::vec2((0.025f + ball_xCenter), (ratio*(0.025f + ball_yCenter)))};         // raquete direita

  
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  // Generate a new VBO and get the associated ID
  glGenBuffers(1, &m_vboVertices);
  // Bind VBO in order to use it
  glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
  // Upload data to VBO
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
               GL_STATIC_DRAW);
  // Unbinding the VBO is allowed (data can be released now)
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &m_vboColors);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
  GLint colorAttribute{glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glEnableVertexAttribArray(colorAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);

  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    const auto size{ImVec2(300, 85)};
    const auto textPosition{ImVec2((m_viewportWidth - size.x) / 2.0f,
                               (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(textPosition); // Posiciona no centro da tela
    ImGui::SetNextWindowSize(size);   // Uma tela de tamanho 300,85
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs}; // Que não tem fundo, nem título, nem é clicável
    ImGui::Begin(" ", nullptr, flags);
    // ImGui::PushFont(m_font);

    // Em alguns estados escreve alguma coisa
    // if (m_gameData.m_state == State::LeftWin) {
      // ImGui::Text("LEFT WINS");
    // } else if (m_gameData.m_state == State::RightWin) {
      // ImGui::Text("RIGHT WINS");
    // } else{
      // ImGui::Text(" ");
    // }

    // ImGui::PopFont();
    ImGui::End();
  }
}


void updatePosBolinha(float* ball_xCenter, float* ball_yCenter, float* ball_xSpeed, float* ball_ySpeed, float* pad_leftCenter, float* pad_rightCenter, float* deltaTime, GameData* m_gameData){

  *ball_xCenter = *ball_xCenter + ((*ball_xSpeed) * (*deltaTime));
  *ball_yCenter = *ball_yCenter + ((*ball_ySpeed) * (*deltaTime));

  if(*ball_xCenter > 2.0f || *ball_xCenter < -2.0f){    // Reseta a bolinha após percorrer uma longa distancia em X

    // if(*ball_xCenter < -2.0f){
      // m_gameData->m_state = State::RightWin;
    // } else {
      // m_gameData->m_state = State::LeftWin;
    // }
    
    *ball_xCenter = 0.0f;
    *ball_yCenter = 0.0f;
    

    std::default_random_engine m_randomEngine;
    m_randomEngine.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> m_randomDist{-0.5f, 0.5f};

    *ball_xSpeed = m_randomDist(m_randomEngine);
    *ball_ySpeed = m_randomDist(m_randomEngine);

  }

  if(*ball_yCenter >=0.91 || *ball_yCenter <= -0.91){    // Colisao com as paredes!

    *ball_ySpeed = (-1) * (*ball_ySpeed);                // Inverte o sentido da velocidade em Y

  }

  if(*ball_xCenter < -0.965f && *ball_xCenter > -1.1f){                          // Colisao com a pad da esquerda

    if(*ball_yCenter >= *pad_leftCenter - 0.2f && *ball_yCenter <= *pad_leftCenter + 0.2f){

      *ball_xSpeed = (-1) * (*ball_xSpeed);

    }

  }
  else if(*ball_xCenter > 0.965f && *ball_xCenter < 1.1f){                     // Colisao com a pad da direita

    if(*ball_yCenter >= *pad_rightCenter - 0.2f && *ball_yCenter <= *pad_rightCenter + 0.2f){

      *ball_xSpeed = (-1) * (*ball_xSpeed);

    }

  }

}

void updatePosPad(float* pad_center, float* pad_velocity, float* deltaTime){

  if(*pad_center > -0.800f && *pad_center < 0.800f ){

    *pad_center = *pad_center + ((*pad_velocity) * (*deltaTime));

  }

}

// void updatePadSpeed(float* pad_velocity){      // Fazer aqui a interação com o input dos jogadores!
void updatePadSpeed(GameData m_gameData, float* pad_leftSpeed, float* pad_rightSpeed){
  
  *pad_leftSpeed=0.0f;
  *pad_rightSpeed=0.0f;

  if(m_gameData.m_input[static_cast<size_t>(Input::LUp)]){
    *pad_leftSpeed=0.5f;
  }
  if(m_gameData.m_input[static_cast<size_t>(Input::LDown)]){
    *pad_leftSpeed=-0.5f;
  }
  if(m_gameData.m_input[static_cast<size_t>(Input::RUp)]){
    *pad_rightSpeed=0.5f;
  }
  if(m_gameData.m_input[static_cast<size_t>(Input::RDown)]){
    *pad_rightSpeed=-0.5f;
  }

}

void OpenGLWindow::paintGL() {

  float deltaTime{static_cast<float>(getDeltaTime())};

  updatePosBolinha(&ball_xCenter, &ball_yCenter, &ball_xSpeed, &ball_ySpeed, &pad_leftCenter, &pad_rightCenter, &deltaTime, &m_gameData);

  updatePadSpeed(m_gameData, &pad_leftSpeed, &pad_rightSpeed);

  updatePosPad(&pad_leftCenter, &pad_leftSpeed, &deltaTime);
  updatePosPad(&pad_rightCenter, &pad_rightSpeed, &deltaTime);

  // Set the clear color
  glClearColor(gsl::at(m_clearColor, 0), gsl::at(m_clearColor, 1),
               gsl::at(m_clearColor, 2), gsl::at(m_clearColor, 3));
  // Clear the color buffer and Z-buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Adjust viewport
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // Start using the shader program
  glUseProgram(m_program);
  // Start using the VAO
  glBindVertexArray(m_vao);


  // Render the game
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    // desenha parede de cima
  glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);    // desenha parede de baixo

  glUniform2f(m_translationLoc, 0, pad_leftCenter);
  glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);    // desenha raquete esquerda

  glUniform2f(m_translationLoc, 0, pad_rightCenter);
  glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);   // desenha raquete direita

  glUniform2f(m_translationLoc, ball_xCenter, ball_yCenter);

  glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);   // desenha bolinha

  glUniform2f(m_translationLoc, 0, 0.0f);

  // End using the VAO
  glBindVertexArray(0);
  // End using the shader program
  glUseProgram(0);
}


void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
}

void OpenGLWindow::terminateGL() {
  // Release OpenGL resources
  glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_vboVertices);
  glDeleteBuffers(1, &m_vboColors);
  glDeleteVertexArrays(1, &m_vao);
}
