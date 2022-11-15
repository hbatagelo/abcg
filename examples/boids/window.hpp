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
    void onResize(glm::ivec2 const &size) override;
private:
    std::mt19937 m_Gen_;
    std::uniform_real_distribution<float> m_Dis_;
    Camera m_Camera_;
    Space m_Space_;
    std::vector<Boid> m_Boids_;
};

#endif
