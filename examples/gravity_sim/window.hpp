#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "particle.hpp"
#include <random>

class Window : public abcg::OpenGLWindow {
protected:
    void onCreate() override;
    void onPaintUI() override;

private:
    bool m_Adding_particle = false;
    std::vector<glm::vec2> m_Mouse_pos_;
    std::vector<Particle> m_Particles_;
    int m_Mass_power_ = 6;
    float m_Mass_;
    glm::vec3 m_Next_color_;
    float m_Gravity_ = 70.f;

    std::mt19937 m_Gen_;
    std::uniform_real_distribution<float> m_Dis_;
};

#endif
