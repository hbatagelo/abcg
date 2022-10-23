#ifndef STAR_HPP_
#define STAR_HPP_

#include "abcgOpenGL.hpp"
#include "shader.hpp"
#include "shape.hpp"

class Star : public Shape<Star> {
public:
    Star(const glm::vec2& position, const glm::vec4& color): Shape(position, color)
    {
    }
    ~Star() {}

    bool draw_ui() override {
        return Shape::draw_ui("Star");
    }

    static void setup(const Shader& shader) {
        std::vector<glm::vec2> points = {
            glm::vec2{ 0.f,      0.2f},
            glm::vec2{ 0.1175f, -0.1618f},
            glm::vec2{-0.0726f, -0.0236f},

            glm::vec2{-0.1175f, -0.1618f},
            glm::vec2{ 0.1902f,  0.0618f},
            glm::vec2{-0.0449f,  0.0618f},

            glm::vec2{-0.1902f,  0.0618f},
            glm::vec2{ 0.0449f,  0.0618f},
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            3, 4, 5,
            6, 7, 1
        };

        Shape::gen_buffers(shader, points, indices);
    }
};

#endif
