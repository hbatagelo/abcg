#include "openglwindow.hpp"

#include <imgui.h>
#include <stdlib.h>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }
}

void OpenGLWindow::initializeGL() {
  // Load fonts
  ImGuiIO &io{ImGui::GetIO()};
  auto filenamePacFont{getAssetsPath() + "pac-font.ttf"};
  auto filenameTimerFont{getAssetsPath() + "timer-font.ttf"};

  m_pac_font = io.Fonts->AddFontFromFileTTF(filenamePacFont.c_str(), 50.0f);
  m_timer_font = io.Fonts->AddFontFromFileTTF(filenameTimerFont.c_str(), 20.0f);

  if (m_pac_font == nullptr || m_timer_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playing;
  m_timeElapsedToWin.restart();

  m_ghost.initializeGL(m_objectsProgram);
  m_pacmans.initializeGL(m_objectsProgram, 5);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_ghost.update(m_gameData);
  m_pacmans.update(deltaTime);

  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
    checkWinCondition();
  }
}

void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_pacmans.paintGL(m_gameData);
  m_ghost.paintGL(m_gameData);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};

    // Display timer
    if (m_gameData.m_state == State::Playing) {
      const auto sizeTimer{ImVec2(m_viewportWidth, 50)};
      const auto positionTimer{ImVec2(10, 10)};
      ImGui::SetNextWindowPos(positionTimer);
      ImGui::SetNextWindowSize(sizeTimer);
      ImGui::Begin(" ", nullptr, flags);

      ImGui::PushFont(m_timer_font);
      ImGui::Text("Timer to WIN: %.1f", m_timeElapsedToWin.elapsed());
      ImGui::Text("Survive for 60s");

      ImGui::PopFont();
      ImGui::End();

      // Display win or lose text
    } else {
      const auto size{ImVec2(440, 85)};
      const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                                 (m_viewportHeight - size.y) / 2.0f)};
      ImGui::SetNextWindowPos(position);
      ImGui::SetNextWindowSize(size);

      ImGui::Begin(" ", nullptr, flags);

      ImGui::PushFont(m_pac_font);

      if (m_gameData.m_state == State::GameOver) {
        ImGui::Text("game over");
      } else if (m_gameData.m_state == State::Win) {
        ImGui::Text("1you win1");
      }

      ImGui::PopFont();
      ImGui::End();
    }
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_objectsProgram);

  m_ghost.terminateGL();
  m_pacmans.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  for (auto &pacman : m_pacmans.m_pacmans) {
    // auto pacmanTranslation{pacman.m_translation};
    // auto distance{glm::distance(m_ghost.m_translation, pacmanTranslation)};

    // if (distance < m_ghost.m_scale * 0.1f + pacman.m_scale * 0.01f) {
    //   m_gameData.m_state = State::GameOver;
    //   m_restartWaitTimer.restart();
    // }
    float minDistanceX = abs(pacman.m_translation.x - m_ghost.m_translation.x);
    float minDistanceY = abs(pacman.m_translation.y - m_ghost.m_translation.y);
    if (minDistanceX <= 0.25f && minDistanceY <= 0.025f) {
      m_gameData.m_state = State::GameOver;
      m_restartWaitTimer.restart();
    }
  }
}

void OpenGLWindow::checkWinCondition() {
  if (m_timeElapsedToWin.elapsed() > 60) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}