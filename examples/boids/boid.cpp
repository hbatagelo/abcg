#include "boid.hpp"
#include <array>

unsigned int Boid::s_EBOSize_ = 0;
GLuint Boid::s_VAO_ = 0;
GLuint Boid::s_VBO_ = 0;
GLuint Boid::s_EBO_ = 0;
GLuint Boid::s_Shader_ = 0;
GLint Boid::s_ModelLocation_ = 0;
GLint Boid::s_ViewLocation_ = 0;
GLint Boid::s_ProjLocation_ = 0;

Boid::Boid() : 
    m_Pos_(0.f, 0.f, 0.f),
    m_Vel_(0.f, 0.f, 0.f),
    m_Acc_(0.f, 0.f, 0.f),
    m_Model_(0.f)
{
}

Boid::~Boid() {
    
}

void Boid::show(const Camera& camera) {
    abcg::glUseProgram(s_Shader_);
    abcg::glBindVertexArray(s_VAO_);

    // abcg::glUniformMatrix4fv(s_ModelLocation_, 0, GL_FALSE, &m_Model_[0][0]);
    // abcg::glUniformMatrix4fv(s_ViewLocation_, 0, GL_FALSE, &camera.getViewMatrix()[0][0]);
    // abcg::glUniformMatrix4fv(s_ProjLocation_, 0, GL_FALSE, &camera.getProjMatrix()[0][0]);

    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO_);
    abcg::glDrawElements(GL_TRIANGLES, s_EBOSize_, GL_UNSIGNED_INT, nullptr);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    abcg::glBindVertexArray(0);
    abcg::glUseProgram(0);
}

void Boid::setup() {
    std::vector<float> vertices = {
        // -0.5f, -0.5f,  0.5f,
        // -0.5f, -0.5f, -0.5f,
        // 0.5f, -0.5f, -0.5f,
        // 0.5f, -0.5f,  0.5f,
        // -0.5f,  0.5f,  0.5f,
        // 0.5f,  0.5f,  0.5f,
        // 0.5f,  0.5f, -0.5f,
        // -0.5f,  0.5f, -0.5f

        -0.5f, 0.5f, 0.f,
        0.5f, 0.5f, 0.f,
        0.5f, -0.5f, 0.f,
    };

    std::vector<unsigned int> indices = {
        0, 2, 1,
        // 2, 0, 3, 
        // 4, 6, 5, 
        // 6, 4, 7, 
        // 0, 5, 3, 
        // 5, 0, 4, 
        // 3, 6, 2, 
        // 6, 3, 5, 
        // 2, 7, 1, 
        // 7, 2, 6, 
        // 1, 4, 0, 
        // 4, 1, 7
    };

    s_EBOSize_ = indices.size();

    const auto assetsPath = abcg::Application::getAssetsPath();
	s_Shader_ = abcg::createOpenGLProgram({
		{.source = assetsPath + "shader.vert", .stage = abcg::ShaderStage::Vertex},
		{.source = assetsPath + "shader.frag", .stage = abcg::ShaderStage::Fragment}
	});
    auto postionLoc = abcg::glGetAttribLocation(s_Shader_, "l_position");

    // s_ModelLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_model");
    // s_ViewLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_view");
    // s_ProjLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_proj");

    abcg::glGenBuffers(0, &s_VBO_);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, s_VBO_);
    abcg::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glGenBuffers(0, &s_EBO_);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO_);
    abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    abcg::glGenVertexArrays(0, &s_VAO_);
    abcg::glBindVertexArray(s_VAO_);
    abcg::glEnableVertexAttribArray(postionLoc);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, s_VBO_);
    abcg::glVertexAttribPointer(postionLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
    abcg::glBindVertexArray(0);
}
