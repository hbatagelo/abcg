#ifndef BOID_HPP_
#define BOID_HPP_

#include "camera.hpp"
#include "abcgOpenGL.hpp"

class Boid {
public:
    Boid();
    ~Boid();

    void show(const Camera& camera);

    static void setup();
private:
    glm::vec3 m_Pos_;
    glm::vec3 m_Vel_;
    glm::vec3 m_Acc_;
    glm::mat4 m_Model_;

    static unsigned int s_EBOSize_;
    static GLuint s_Shader_;
    static GLuint s_VAO_;
    static GLuint s_VBO_;
    static GLuint s_EBO_;
    static GLint s_ModelLocation_;
    static GLint s_ViewLocation_;
    static GLint s_ProjLocation_;
};

#endif
