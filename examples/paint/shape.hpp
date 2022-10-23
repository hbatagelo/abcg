#ifndef SHAPE_HPP_
#define SHAPE_HPP_

#include "abcgOpenGL.hpp"
#include "shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

class BaseShape {
public:
    virtual void draw() = 0;
    virtual bool draw_ui() = 0;
};

template<class DerivedShape>
class Shape : public BaseShape {
public:
    Shape(const glm::vec2& position, const glm::vec4& color) : 
        m_Position_(position), 
        m_Color_(color)
    {
        calc_model();
    }

    ~Shape() {}

    void draw() override {
        abcg::glUseProgram(m_Shader_->program);
        abcg::glBindVertexArray(m_VAO_);

        abcg::glUniformMatrix4fv(m_Shader_->modelULoc, 1, GL_FALSE, glm::value_ptr(m_Model_));
        abcg::glUniform4fv(m_Shader_->colorlULoc, 1, &m_Color_.r);


        abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_);
        abcg::glDrawElements(GL_TRIANGLES, m_EBOSize_, GL_UNSIGNED_INT, nullptr);
        abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        abcg::glBindVertexArray(0);
        abcg::glUseProgram(0);
    }

    bool draw_ui() override { return false; }
protected:
    bool draw_ui(const std::string& header_name) {
        if (ImGui::CollapsingHeader(header_name.c_str(), &m_Show_))
        {
            ImGui::ColorEdit4("Color", &m_Color_.r, ImGuiColorEditFlags_NoInputs);
            if (
                ImGui::SliderFloat2("Position", &m_Position_.x, -1.f, 1.f) ||
                ImGui::SliderFloat2("Scale", &m_Scale_.x, -5.f, 5.f) ||
                ImGui::SliderFloat("Rotation", &m_Rotation_, 0, 360.f)
            ) {
                calc_model();
            }
        }
        return m_Show_;
    }

    static void gen_buffers(const Shader& shader, const std::vector<glm::vec2>& points, const std::vector<unsigned int>& indices) {
        m_EBOSize_ = indices.size();
        m_Shader_ = &shader;

        abcg::glGenBuffers(1, &m_VBO_);
        abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO_);
        abcg::glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec2), points.data(), GL_STATIC_DRAW);
        abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

        abcg::glGenBuffers(1, &m_EBO_);
        abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_);
        abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        abcg::glGenVertexArrays(1, &m_VAO_);
        abcg::glBindVertexArray(m_VAO_);
        abcg::glEnableVertexAttribArray(m_Shader_->positionALoc);
        abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO_);
        abcg::glVertexAttribPointer(m_Shader_->positionALoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
        abcg::glBindVertexArray(0);
    }

    void calc_model() {
        m_Model_ = glm::mat4(1.f);
        m_Model_ = glm::translate(m_Model_, glm::vec3(m_Position_, 0.0f));
        m_Model_ = glm::rotate(m_Model_, glm::radians(m_Rotation_), glm::vec3(0.0f, 0.0f, 1.0f));
        m_Model_ = glm::scale(m_Model_, glm::vec3(m_Scale_, 1.f));
    }

    static GLuint m_VAO_;
    static GLuint m_VBO_;
    static GLuint m_EBO_;
    static unsigned int m_EBOSize_;
    static const Shader* m_Shader_;

    glm::vec2 m_Position_;
    glm::vec2 m_Scale_ = glm::vec2(1.f);
    float m_Rotation_ = 0.f;
    
    glm::vec4 m_Color_;
    glm::mat4 m_Model_ = glm::mat4(1.f);
    bool m_Show_ = true;
};

#endif
