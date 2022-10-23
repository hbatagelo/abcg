#ifndef PENTAGON_HPP_
#define PENTAGON_HPP_

#include "abcgOpenGL.hpp"
#include "shader.hpp"
#include "shape.hpp"

class Pentagon : public Shape<Pentagon> {
public:
    Pentagon(const glm::vec2& position, const glm::vec4& color): Shape(position, color)
    {
    }
    ~Pentagon() {}

    bool draw_ui() override {
        return Shape::draw_ui("Pentagon");
    }

    static void setup(const Shader& shader) {
        std::vector<glm::vec2> points = {
            glm::vec2{0.f, 0.f}, 
            glm::vec2{0.f, 0.2f},
            glm::vec2{-0.1902f,  0.0618f},
            glm::vec2{-0.1175f, -0.1618f},
            glm::vec2{ 0.1175f, -0.1618f},
            glm::vec2{ 0.1902f,  0.0618f},
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 5,
            0, 5, 1
        };

        Shape::gen_buffers(shader, points, indices);
    }
};

#endif
