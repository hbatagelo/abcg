#ifndef BOID_HPP_
#define BOID_HPP_

#include <vector>
#include <iostream>
#include "light.hpp"
#include "camera.hpp"
#include "space.hpp"
#include "abcgOpenGL.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;

    friend bool operator==(const Vertex&, const Vertex&) = default;
};

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
    size_t operator()(const Vertex& vertex) const noexcept {
        const auto h1 = std::hash<glm::vec3>()(vertex.position);
        const auto h2 = std::hash<glm::vec3>()(vertex.normal);
        const auto h3 = std::hash<glm::vec2>()(vertex.texCoord);
        return abcg::hashCombine(h1, h2, h3);
    }
};

class Boid {
public:
    //NOTE: Force = Acceleration, since Mass = 1

    Boid(const glm::vec3& pos, const glm::vec3& vel);
    ~Boid();

    //Check if the boid is outside the world and repostion it
    void checkEdge(float worldSize);

    //Simulate the flocking behaviour
    void simulate(const std::vector<Boid>& boids);

    //Update the position and velocity
    void update(float dt);

    //Render
    void show(const Camera& camera, Space& space, bool useDirLight, bool usePointLight, bool useSpotLight);

    //Generate the OpenGL Buffers
    static void setup(Space& space);    

    //Render the ImGui
    static void showUI(int id);
    static float maxVel() { return s_MaxVel_; }
private:
    //Calculate the steering force based on disired target
    glm::vec3 steer(const glm::vec3& desired);

    static void standardize(std::vector<Vertex>& vertices);
    static std::pair<std::vector<Vertex>, std::vector<unsigned int>> loadModelFromFile(std::string_view path, bool normalize);

    //Calculate he Model Matrix
    void calcModelMatrix();

    //Boid position in the world
    glm::vec3 m_Pos_;
    //Boid velocity
    glm::vec3 m_Vel_;
    //Boid steering acceleration
    glm::vec3 m_Acc_;
    //Model matrix
    glm::mat4 m_Model_;


    //Maximum velocity all boids in the simulation can go
    static float s_AlignmentMult_;
    //Perception radius of the boid
    static float s_CohesionMult_;
    //Maximum steering force
    static float s_SeparationMult_;

    //Maximum velocity all boids in the simulation can go
    static float s_MaxVel_;
    //Perception radius of the boid
    static float s_Perception_;
    //Maximum steering force
    static float s_MaxForce_;

    static unsigned int s_EBOSize_;
    static GLuint s_Shader_;
    static GLuint s_VAO_;
    static GLuint s_VBO_;
    static GLuint s_EBO_;

    static GLint s_ModelLocation_;
    static GLint s_ViewLocation_;
    static GLint s_ProjLocation_;
    static GLint s_CamPositionLocation_;
    static GLint s_UseDirLightLocation_;
    static GLint s_UsePointLightLocation_;
    static GLint s_UseSpotLightLocation_;

    //Boid material used for by the texture
    struct Material : public LightProperty {
        Material() {}

        void showUI(int id) {
            ImGui::PushID(id);
            ImGui::Separator();
            LightProperty::showUI();
            ImGui::SliderFloat("Shininess", &shininess, 0.f, 100.f);
            ImGui::PopID();
        }

        unsigned int diffuseTexture;
        float shininess;

        GLint LocationDiffuseTexture;
        GLint LocationShininess;
    };
    static Material s_Material_;
};

#endif
