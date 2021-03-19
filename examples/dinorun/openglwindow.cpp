#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE) {
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
      m_gameData.m_state = State::Playing;
    }
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.set(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }

}

void OpenGLWindow::initializeGL() {
  // Create program to render the stars
  m_starsProgram = createProgramFromFile(getAssetsPath() + "stars.vert",
                                         getAssetsPath() + "stars.frag");
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::NotRunning;
  m_starLayers.initializeGL(m_starsProgram, 25);
  m_dino.initializeGL(m_objectsProgram);
  m_asteroids.initializeGL(m_objectsProgram, 3);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::NotRunning &&
      m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_dino.update(m_gameData, deltaTime);
  m_starLayers.update(m_dino, deltaTime);
  m_asteroids.update(m_dino, deltaTime);
  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
    checkWinCondition();
  }
}

void OpenGLWindow::paintGL() {
  update();

  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  m_starLayers.paintGL();
  m_asteroids.paintGL();
  m_dino.paintGL(m_gameData);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    auto size{ImVec2(300, 85)};
    auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                         (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("*You Win!*");
    } else if (m_gameData.m_state == State::NotRunning) {
      ImGui::Text("Pressspace");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}
void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_starsProgram);
  glDeleteProgram(m_objectsProgram);

  m_asteroids.terminateGL();
  m_dino.terminateGL();
  m_starLayers.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  // Check collision between dino and asteroids
  for (auto &asteroid : m_asteroids.m_asteroids) {
    auto asteroidTranslation{asteroid.m_translation};
    auto distance{glm::distance(m_dino.m_translation, asteroidTranslation)};


    if (distance < m_dino.m_scale * 0.125f + asteroid.m_scale * 0.85f) {
      m_gameData.m_state = State::GameOver;
      m_restartWaitTimer.restart();
    }
  }

}

void OpenGLWindow::checkWinCondition() {
  if (m_asteroids.m_asteroids.empty()) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}