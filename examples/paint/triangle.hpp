#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include "abcgOpenGL.hpp"
#include "shader.hpp"
#include "shape.hpp"

class Triangle : public Shape<Triangle> {
public:
    Triangle(const glm::vec2& position, const glm::vec4& color) : Shape(position, color)
    {
    }
    ~Triangle() {}

    bool draw_ui() override {
        return Shape::draw_ui("Triangle");
    }

    static void setup(const Shader& shader) {
        std::vector<glm::vec2> points = {
            glm::vec2{-.2f, -.2f}, glm::vec2{.2f, -.2f}, glm::vec2{0.f, .2f}
        };

        std::vector<unsigned int> indices = {
            1, 2, 0,
        };

        Shape::gen_buffers(shader, points, indices);
    }
};

#endif
