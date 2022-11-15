#include "boid.hpp"
#include <array>

float Boid::s_Perception_ = 30.f;
float Boid::s_MaxForce_ = 10.f;
float Boid::s_MaxVel_ = 30.f;
unsigned int Boid::s_EBOSize_ = 0;
GLuint Boid::s_VAO_ = 0;
GLuint Boid::s_VBO_ = 0;
GLuint Boid::s_EBO_ = 0;
GLuint Boid::s_Shader_ = 0;
GLint Boid::s_ModelLocation_ = 0;
GLint Boid::s_ViewLocation_ = 0;
GLint Boid::s_ProjLocation_ = 0;

Boid::Boid(const glm::vec3& pos, const glm::vec3& vel) : 
    m_Pos_(pos),
    m_Vel_(vel),
    m_Acc_(0.f, 0.f, 0.f)
{
    calcModelMatrix();
}

Boid::~Boid() {
    
}

void Boid::calcModelMatrix() {
    m_Model_ = glm::mat4(1.f);
    m_Model_ = glm::translate(m_Model_, m_Pos_);
    m_Model_ = glm::scale(m_Model_, glm::vec3(2.5f));
}

void Boid::show(const Camera& camera) {
    abcg::glUseProgram(s_Shader_);
    abcg::glBindVertexArray(s_VAO_);

    abcg::glUniformMatrix4fv(s_ModelLocation_, 1, GL_FALSE, &m_Model_[0][0]);
    abcg::glUniformMatrix4fv(s_ViewLocation_, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
    abcg::glUniformMatrix4fv(s_ProjLocation_, 1, GL_FALSE, &camera.getProjMatrix()[0][0]);

    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO_);
    abcg::glDrawElements(GL_TRIANGLES, s_EBOSize_, GL_UNSIGNED_INT, nullptr);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    abcg::glBindVertexArray(0);
    abcg::glUseProgram(0);
}

void Boid::checkEdge(float worldSize) {
    //Check if the current position is outside the
    //box and reposition it

    if (m_Pos_.x > worldSize) {
        m_Pos_.x = -worldSize;
    } else if (m_Pos_.x < -worldSize) {
        m_Pos_.x = worldSize;
    }

    if (m_Pos_.y > worldSize) {
        m_Pos_.y = -worldSize;
    } else if (m_Pos_.y < -worldSize) {
        m_Pos_.y = worldSize;
    }

    if (m_Pos_.z > worldSize) {
        m_Pos_.z = -worldSize;
    } else if (m_Pos_.z < -worldSize) {
        m_Pos_.z = worldSize;
    }
}

glm::vec3 Boid::steer(const glm::vec3& desired) {
    //Make the magnitude the vector equal to s_MaxVel_
    glm::vec3 steerVec = glm::normalize(desired) * s_MaxVel_;
    //Steer toward the disired target
    steerVec -= m_Vel_;
    //Clamp the vector so it doesn't go above s_MaxForce_
    if (glm::length(steerVec) > s_MaxForce_) {
        steerVec = glm::normalize(steerVec) * s_MaxForce_;
    }
    return steerVec;
}

void Boid::simulate(const std::vector<Boid>& boids) {
    //The simulation consists of three parts:
    //  Alignment: Avg velocity of neighbour boids
    //  Cohesion: Avg position of neighbour boids
    //  Separation: Avoid collision (inversely proportional to distance)

    m_Acc_ = glm::vec3(0.f);
    glm::vec3 avgAlignment(0.f);
    glm::vec3 avgCohesion(0.f);
    glm::vec3 avgSeparation(0.f);

    //Loop over all the boids and collect the desired information
    //from those who are inside the sphere of perception radius
    //and are different from the current boid
    unsigned int total = 0;
    for (const auto& b : boids) {
        float d = glm::distance(m_Pos_, b.m_Pos_);
        if (&b != this && d < s_Perception_) {
            avgAlignment += b.m_Vel_;
            avgCohesion += b.m_Pos_;
            avgSeparation += (m_Pos_ - b.m_Pos_)/(d*d);
            total++;
        }
    }

    if (!total)
        return;

    //Takes the average
    avgAlignment /= (float)total;
    avgCohesion /= (float)total;
    avgSeparation /= (float)total;

    //Cohesion depends on the current boid position
    avgCohesion -= m_Pos_;

    //Steer towards each part of the simulation and accumulate in the acceleration
    m_Acc_ += steer(avgAlignment) + steer(avgCohesion) + steer(avgSeparation);
}

void Boid::update(float dt) {
    //Update the position and velocity
    m_Pos_ += dt * m_Vel_;
    m_Vel_ += dt * m_Acc_;
    
    //Clamp the velocity to s_MaxVel_
    if (glm::length(m_Vel_) > s_MaxVel_) {
        m_Vel_ = glm::normalize(m_Vel_) * s_MaxVel_;
    }

    //Reset the acceleration
    m_Acc_ = glm::vec3(0.f);

    calcModelMatrix();
}

void Boid::setup() {
    std::vector<float> vertices = {
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f
    };

    std::vector<unsigned int> indices = {
        0, 2, 1,
        2, 0, 3, 
        4, 6, 5, 
        6, 4, 7, 
        0, 5, 3, 
        5, 0, 4, 
        3, 6, 2, 
        6, 3, 5, 
        2, 7, 1, 
        7, 2, 6, 
        1, 4, 0, 
        4, 1, 7
    };

    s_EBOSize_ = indices.size();

    const auto assetsPath = abcg::Application::getAssetsPath();
	s_Shader_ = abcg::createOpenGLProgram({
		{.source = assetsPath + "shader.vert", .stage = abcg::ShaderStage::Vertex},
		{.source = assetsPath + "boid.frag", .stage = abcg::ShaderStage::Fragment}
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

void Boid::showUI() {
    if (!ImGui::CollapsingHeader("Boids"))
        return;
    ImGui::DragFloat("Perception Radius", &s_Perception_);
    ImGui::DragFloat("Maximum Force", &s_MaxForce_);
    ImGui::DragFloat("Velocity", &s_MaxVel_);
}
