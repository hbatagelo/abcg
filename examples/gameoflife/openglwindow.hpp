#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <array>
#include "grid.hpp"

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
    protected:
        void handleEvent(SDL_Event& event) override;
        void initializeGL() override;
        void paintGL() override;
        void paintUI() override;
        void resizeGL(int width, int height) override;
        void terminateGL() override;

    private:
        GLuint m_vao{};
        GLuint m_vboVertices{};
        GLuint m_vboColors{};
        GLuint m_program{};

        int m_viewportWidth{};
        int m_viewportHeight{};

        Grid m_grid;

        int m_speed;
        abcg::ElapsedTimer m_restartWaitTimer;

        void restart();
        void update();
};

#endif