#include "openglwindow.hpp"

#include <imgui.h>

#include "abcg.hpp"

////////// --------------------------------------- EVENT HANDLER -------------------------------------//////////////
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

////////// --------------------------------------- INITIALIZE -------------------------------------//////////////
void OpenGLWindow::initializeGL() {
  // Não usaremos uma fonte diferente por enquanto
  // Load a new font
  // ImGuiIO &io{ImGui::GetIO()};
  // auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  // m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  // if (m_font == nullptr) {
  //   throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  // }

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  abcg::glClearColor(0, 0, 0, 0);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  // m_randomEngine.seed(
      // std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

////////// --------------------------------------- RESTART -------------------------------------//////////////
void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playing;

  glm::vec2 leftStartPosition{-0.8f,0.0f};
  glm::vec2 rightStartPosition{0.8f,0.0f};
  m_padLeft.initializeGL(m_objectsProgram, leftStartPosition, 1);
  m_padRight.initializeGL(m_objectsProgram, rightStartPosition, 2);
  // talvez fazer isso com a bolinha
}

////////// --------------------------------------- UPDATE -------------------------------------//////////////
void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())}; // getDeltaTime é importante

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_padLeft.update(m_gameData, deltaTime);
  m_padRight.update(m_gameData, deltaTime);
}

////////// --------------------------------------- PAINT GL -------------------------------------//////////////
void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_padLeft.paintGL();
  m_padRight.paintGL();
}

////////// --------------------------------------- PAINT UI -------------------------------------//////////////
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
    if (m_gameData.m_state == State::WinLeft) {
      ImGui::Text("LEFT WINS");
    } else if (m_gameData.m_state == State::WinRight) {
      ImGui::Text("RIGHT WINS");
    }

    // ImGui::PopFont();
    ImGui::End();
  }
}

///////// --------------------------------------- Resize -------------------------------------//////////////
void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

////////// --------------------------------------- Terminate -------------------------------------//////////////
void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_objectsProgram);

  m_padLeft.terminateGL();
  m_padRight.terminateGL();
}