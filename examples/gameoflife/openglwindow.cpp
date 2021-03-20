#include "openglwindow.hpp"

#include <fmt/core.h>

#include <imgui.h>
#include <algorithm>
#include <cppitertools/itertools.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <gsl/gsl>


#include "abcg.hpp"

void OpenGLWindow::initializeGL() {
    terminateGL();

    m_program = createProgramFromFile(getAssetsPath() + "objects.vert", getAssetsPath() + "objects.frag");

    glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif

    restart();
}

void OpenGLWindow::paintGL() {
    update();

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, m_viewportWidth, m_viewportHeight);

    m_grid.paintGL();
}

void OpenGLWindow::paintUI() {
    // abcg::OpenGLWindow::paintUI();
    {
        auto widgetSize{ImVec2(200, 72)};
        ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5,
                                    m_viewportHeight - widgetSize.y - 5));
        ImGui::SetNextWindowSize(widgetSize);
        auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoTitleBar};
        ImGui::Begin(" ", nullptr, windowFlags);

        ImGui::PushItemWidth(140);
        ImGui::SliderInt("Delay", &m_speed, 1, 1000, "%d ms");
        ImGui::PopItemWidth();

        ImGui::Columns(2);
        if (ImGui::Button("Clear", ImVec2(-1, 30))) {
            m_grid.clearGrid();
        }
        ImGui::NextColumn();
        if (ImGui::Button("Restart", ImVec2(-1, 30))) {
            m_grid.restart();
        }
    }

    ImGui::End();
}

void OpenGLWindow::resizeGL(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
}

void OpenGLWindow::restart() {
    m_grid.initializeGL(m_program);
}

void OpenGLWindow::terminateGL() {
    // Release OpenGL resources
    glDeleteProgram(m_program);
    glDeleteBuffers(1, &m_vboVertices);
    glDeleteBuffers(1, &m_vboColors);
    glDeleteVertexArrays(1, &m_vao);
}

void OpenGLWindow::update() {
    if (m_restartWaitTimer.elapsed() > m_speed / 1000.0) {
        m_grid.update();
        m_restartWaitTimer.restart();
    }
}

void OpenGLWindow::handleEvent(SDL_Event &event) {
    // Keyboard events
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE)
            m_grid.game.toggleState();
    }

    // Mouse events
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            glm::ivec2 mousePosition;
            SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
            glm::vec2 direction{glm::vec2{mousePosition.x - m_viewportWidth / 2, mousePosition.y - m_viewportHeight / 2}};
            direction.y = -direction.y;
            m_grid.toggleCellState(2 * direction.x / m_viewportWidth, 2 * direction.y / m_viewportHeight);
        }
    }
}
