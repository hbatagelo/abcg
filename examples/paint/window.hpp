#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <memory>
#include "abcgOpenGL.hpp"
#include "shader.hpp"
#include "shape.hpp"
#include "rectangle.hpp"
#include "circle.hpp"
#include "triangle.hpp"
#include "pentagon.hpp"
#include "star.hpp"

class Window : public abcg::OpenGLWindow {
protected:
    void create_shaders();

    void onCreate() override;
    void onPaintUI() override;
    void onPaint() override;
    void onEvent(SDL_Event const &event) override;
    void onResize(glm::ivec2 const &size);

private:
    enum SapeTypes {
        Rectangle,
        Circle,
        Triangle,
        Pentagon,
        Star,
    };
    int m_SelectedShape_ = 5;
    std::vector<const char*> m_ShapeNames_ = {
        "Rectangle",
        "Circle",
        "Triangle",
        "Pentagon",
        "Star",
        "Disabled"
    };

    glm::vec4 m_Color_ = glm::vec4(0.197f, 0.131f, 0.456f, 1.f);
    glm::vec3 m_BakgroundColor_ = glm::vec3(0.3f, 0.3f, 0.3f);

    std::vector<std::unique_ptr<BaseShape>> m_Shapes_;
    glm::ivec2 m_Viewport_;

    Shader m_DefaultShader_;
    Shader m_CircleShader_;
};

#endif
