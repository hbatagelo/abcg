#ifndef RECTANGLE_HPP_
#define RECTANGLE_HPP_

#include "abcgOpenGL.hpp"
#include "shader.hpp"
#include "shape.hpp"

class Rectangle : public Shape<Rectangle> {
public:
    Rectangle(const glm::vec2& position, const glm::vec4& color) : Shape(position, color)
    {
    }
    ~Rectangle() {}

    bool draw_ui() override {
        return Shape::draw_ui("Rectangle");
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
