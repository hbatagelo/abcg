#ifndef CIRCLE_HPP_
#define CIRCLE_HPP_

#include "abcgOpenGL.hpp"
#include "shader.hpp"
#include "shape.hpp"

class Circle : public Shape<Circle> {
public:
    Circle(const glm::vec2& position, const glm::vec4& color) : Shape(position, color)
    {
    }
    ~Circle() {}

    bool draw_ui() override {
        return Shape::draw_ui("Circle");
    }

    static void setup(const Shader& shader) {
        std::vector<glm::vec2> points = {
            glm::vec2{-.2f, -.2f}, glm::vec2{.2f, -.2f}, glm::vec2{.2f, .2f}, glm::vec2{-.2f, .2f}
        };

        std::vector<unsigned int> indices = {
            1, 2, 0,
            2, 3, 0
        };

        Shape::gen_buffers(shader, points, indices);
    }
};

#endif
