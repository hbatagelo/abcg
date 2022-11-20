#include "boid.hpp"
#include <array>

float Boid::s_AlignmentMult_ = 1.f;
float Boid::s_CohesionMult_ = 1.f;
float Boid::s_SeparationMult_ = 1.f;
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
    auto geometryFront = glm::vec3(0.0f, 0.f, 1.f);
    auto normVel = glm::normalize(m_Vel_);
    auto rotationVec = glm::cross(geometryFront, normVel);
    auto angle = glm::acos(glm::dot(geometryFront, normVel));

    m_Model_ = glm::mat4(1.f);
    m_Model_ = glm::translate(m_Model_, m_Pos_);
    if (glm::length2(rotationVec) != 0.f) {
        m_Model_ = glm::rotate(m_Model_, angle, rotationVec);
    }
    m_Model_ = glm::scale(m_Model_, glm::vec3(7.f));
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
    m_Acc_ += s_AlignmentMult_ * steer(avgAlignment) + s_CohesionMult_ * steer(avgCohesion) + s_SeparationMult_ * steer(avgSeparation);
}

void Boid::update(float dt) {
    //Update the position and velocity
    m_Pos_ += dt * m_Vel_;
    calcModelMatrix();

    m_Vel_ += dt * m_Acc_;
    
    //Clamp the velocity to s_MaxVel_
    if (glm::length(m_Vel_) > s_MaxVel_) {
        m_Vel_ = glm::normalize(m_Vel_) * s_MaxVel_;
    }

    //Reset the acceleration
    m_Acc_ = glm::vec3(0.f);
}

void Boid::setup() {
    const auto assetsPath = abcg::Application::getAssetsPath();
    auto [vertices, indices] = loadModelFromFile(assetsPath + "bird.obj", true);

    s_EBOSize_ = indices.size();

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
    abcg::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
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
    ImGui::DragFloat("Aligment", &s_AlignmentMult_, 0.1f, 0.f, 10.f);
    ImGui::DragFloat("Cohesion", &s_CohesionMult_, 0.1f, 0.f, 10.f);
    ImGui::DragFloat("Separation", &s_SeparationMult_, 0.1f, 0.f, 10.f);
    ImGui::DragFloat("Perception Radius", &s_Perception_, 0.1f, 0.f, 10.f);
    ImGui::DragFloat("Maximum Force", &s_MaxForce_);
    ImGui::DragFloat("Velocity", &s_MaxVel_);
}

std::pair<std::vector<Vertex>, std::vector<unsigned int>> Boid::loadModelFromFile(std::string_view path, bool normalize) {
    tinyobj::ObjReader reader;

    // std::vector<float> vertices;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    if (!reader.ParseFromFile(path.data())) {
        if (!reader.Error().empty()) {
            throw abcg::RuntimeError(
                fmt::format("Failed to load model {} ({})", path, reader.Error()));
        }
        throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
    }

    if (!reader.Warning().empty()) {
        fmt::print("Warning: {}\n", reader.Warning());
    }

    std::unordered_map<Vertex, unsigned int> hash{};
    auto const &attrib{reader.GetAttrib()};
    auto const &shapes{reader.GetShapes()};

    // Loop over shapes
    for (auto const &shape : shapes) {
        // Loop over indices
        for (auto const offset : iter::range(shape.mesh.indices.size())) {
            // Access to vertex
            auto const index{shape.mesh.indices.at(offset)};

            // Vertex position
            const auto startIndex = 3 * index.vertex_index;
            const auto vx = attrib.vertices.at(startIndex + 0);
            const auto vy = attrib.vertices.at(startIndex + 1);
            const auto vz = attrib.vertices.at(startIndex + 2);

            const Vertex vertex{.position = {vx, vy, vz}};

            // If map doesn't contain this vertex
            if (!hash.contains(vertex)) {
                // Add this index (size of m_vertices)
                hash[vertex] = vertices.size();
                // Add this vertex
                vertices.push_back(vertex);
            }
            indices.push_back(hash[vertex]);
        }
    }

    if (normalize)
        standardize(vertices);

    return {vertices, indices};
}

void Boid::standardize(std::vector<Vertex>& vertices) {
    // Center to origin and normalize bounds to [-1, 1]
    // Get bounds
    glm::vec3 max(std::numeric_limits<float>::lowest());
    glm::vec3 min(std::numeric_limits<float>::max());
    for (const auto& vertex : vertices) {
        max = glm::max(max, vertex.position);
        min = glm::min(min, vertex.position);
    }

    // Center and scale
    auto const center = (min + max) / 2.0f;
    auto const scaling = 2.0f / glm::length(max - min);
    for (auto& vertex : vertices) {
        vertex.position = (vertex.position - center) * scaling;
    }
}
