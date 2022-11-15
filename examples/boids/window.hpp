#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "space.hpp"
#include "boid.hpp"
#include "camera.hpp"
#include <random>
#include <vector>

class Window : public abcg::OpenGLWindow {
protected:
    void onCreate() override;
    void onPaintUI() override;
    void onPaint() override;
    void onUpdate() override;
    void onResize(glm::ivec2 const &size) override;
    void onEvent(SDL_Event const &event) override;

    void createBoids();
private:
    int m_NumberOfBoids_ = 300;
    abcg::Timer m_Timer_;
    Camera m_Camera_;
    Space m_Space_;
    std::vector<Boid> m_Boids_;
};

#endif
