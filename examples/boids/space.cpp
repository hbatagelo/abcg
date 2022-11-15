#include "space.hpp"
#include <array>

unsigned int Space::s_EBOSize_ = 0;
GLuint Space::s_VAO_ = 0;
GLuint Space::s_VBO_ = 0;
GLuint Space::s_EBO_ = 0;
GLuint Space::s_Shader_ = 0;
GLint Space::s_ModelLocation_ = 0;
GLint Space::s_ViewLocation_ = 0;
GLint Space::s_ProjLocation_ = 0;

Space::Space() : m_Model_(glm::scale(glm::mat4(1.f), glm::vec3(m_Size_)))
{
}

Space::~Space() {
    
}

void Space::show(const Camera& camera) {
    abcg::glLineWidth(3.0);
    abcg::glUseProgram(s_Shader_);
    abcg::glBindVertexArray(s_VAO_);

    abcg::glUniformMatrix4fv(s_ModelLocation_, 1, GL_FALSE, &m_Model_[0][0]);
    abcg::glUniformMatrix4fv(s_ViewLocation_, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
    abcg::glUniformMatrix4fv(s_ProjLocation_, 1, GL_FALSE, &camera.getProjMatrix()[0][0]);

    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO_);
    abcg::glDrawElements(GL_LINES, s_EBOSize_, GL_UNSIGNED_INT, nullptr);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    abcg::glBindVertexArray(0);
    abcg::glUseProgram(0);
    abcg::glLineWidth(1.0);
}

void Space::setup() {
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
        0, 1,
        1, 2,
        2, 3,
        3, 0,

        3, 5,
        5, 6,
        6, 2,

        4, 5,
        4, 7,
        6, 7,

        1, 7,
        0, 4
    };

    s_EBOSize_ = indices.size();

    const auto assetsPath = abcg::Application::getAssetsPath();
	s_Shader_ = abcg::createOpenGLProgram({
		{.source = assetsPath + "shader.vert", .stage = abcg::ShaderStage::Vertex},
		{.source = assetsPath + "space.frag", .stage = abcg::ShaderStage::Fragment}
	});
    auto postionLoc = abcg::glGetAttribLocation(s_Shader_, "l_position");

    s_ModelLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_model");
    s_ViewLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_view");
    s_ProjLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_proj");

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
    abcg::glEnableVertexAttribArray(postionLoc);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, s_VBO_);
    abcg::glVertexAttribPointer(postionLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
    abcg::glBindVertexArray(0);
}
