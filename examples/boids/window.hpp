#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "space.hpp"
#include "boid.hpp"
#include "camera.hpp"
#include <random>
#include <vector>
#include <memory>

class Window : public abcg::OpenGLWindow {
protected:
    void onCreate() override;
    void onPaintUI() override;
    void onPaint() override;
    void onResize(const glm::ivec2& size) override;
    void onEvent(const SDL_Event& event) override;
    void createBoids();

private:
    std::unique_ptr<Shader> m_Shader_;
    int m_NumberOfBoids_ = 150;
    abcg::Timer m_Timer_;
    Camera m_Camera_;
    Space m_Space_;
    std::vector<Boid> m_Boids_;
};

#endif
