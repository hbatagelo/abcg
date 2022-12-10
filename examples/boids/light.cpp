#include "light.hpp"

unsigned int PointLight::s_EBOSize_ = 0;
GLuint PointLight::s_VAO_ = 0;
GLuint PointLight::s_VBO_ = 0;
GLuint PointLight::s_EBO_ = 0;

unsigned int SpotLight::s_EBOSize_ = 0;
GLuint SpotLight::s_VAO_ = 0;
GLuint SpotLight::s_VBO_ = 0;
GLuint SpotLight::s_EBO_ = 0;

void PointLight::setup(const Shader& shader) {
    std::vector<float> vertices = {
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3,

        2, 3, 6,
        3, 5, 6,

        5, 6, 7,
        4, 5, 7,

        0, 1, 4,
        1, 4, 7,

        0, 3, 4,
        3, 4, 5,

        1, 2, 6,
        1, 6, 7
    };

    s_EBOSize_ = indices.size();

    abcg::glGenBuffers(1, &s_VBO_);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, s_VBO_);
    abcg::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glGenBuffers(1, &s_EBO_);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO_);
    abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    abcg::glGenVertexArrays(1, &s_VAO_);
    abcg::glBindVertexArray(s_VAO_);
    abcg::glEnableVertexAttribArray(shader.positionALoc);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, s_VBO_);
    abcg::glVertexAttribPointer(shader.positionALoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
    abcg::glBindVertexArray(0);
}

void PointLight::show(const Camera& camera, const Shader& shader) {
    abcg::glUseProgram(shader.program);
    abcg::glBindVertexArray(s_VAO_);

    abcg::glUniformMatrix4fv(shader.modelULoc, 1, GL_FALSE, &m_Model_[0][0]);
    abcg::glUniformMatrix4fv(shader.viewULoc, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
    abcg::glUniformMatrix4fv(shader.projULoc, 1, GL_FALSE, &camera.getProjMatrix()[0][0]);
    abcg::glUniform1i(shader.stripULoc, 1);
    abcg::glUniform3fv(shader.AcolorULoc, 1, &ambient[0]);
    abcg::glUniform3fv(shader.DcolorULoc, 1, &diffuse[0]);
    abcg::glUniform3fv(shader.ScolorULoc, 1, &specular[0]);

    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO_);
    abcg::glDrawElements(GL_TRIANGLES, s_EBOSize_, GL_UNSIGNED_INT, nullptr);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    abcg::glBindVertexArray(0);
    abcg::glUseProgram(0);
}

void SpotLight::setup(const Shader& shader) {
    std::vector<float> vertices = {
        0.f, -1.f, 0.f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 1, 4,
        0, 2, 3,
        0, 3, 4,

        1, 2, 3,
        1, 3, 4
    };

    s_EBOSize_ = indices.size();

    abcg::glGenBuffers(1, &s_VBO_);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, s_VBO_);
    abcg::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glGenBuffers(1, &s_EBO_);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO_);
    abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    abcg::glGenVertexArrays(1, &s_VAO_);
    abcg::glBindVertexArray(s_VAO_);
    abcg::glEnableVertexAttribArray(shader.positionALoc);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, s_VBO_);
    abcg::glVertexAttribPointer(shader.positionALoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
    abcg::glBindVertexArray(0);
}

void SpotLight::show(const Camera& camera, const Shader& shader) {
    abcg::glUseProgram(shader.program);
    abcg::glBindVertexArray(s_VAO_);

    abcg::glUniformMatrix4fv(shader.modelULoc, 1, GL_FALSE, &m_Model_[0][0]);
    abcg::glUniformMatrix4fv(shader.viewULoc, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
    abcg::glUniformMatrix4fv(shader.projULoc, 1, GL_FALSE, &camera.getProjMatrix()[0][0]);
    abcg::glUniform1i(shader.stripULoc, 1);
    abcg::glUniform3fv(shader.AcolorULoc, 1, &ambient[0]);
    abcg::glUniform3fv(shader.DcolorULoc, 1, &diffuse[0]);
    abcg::glUniform3fv(shader.ScolorULoc, 1, &specular[0]);

    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO_);
    abcg::glDrawElements(GL_TRIANGLES, s_EBOSize_, GL_UNSIGNED_INT, nullptr);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    abcg::glBindVertexArray(0);
    abcg::glUseProgram(0);
}
