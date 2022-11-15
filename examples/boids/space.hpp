#ifndef SPACE_HPP_
#define SPACE_HPP_

#include "camera.hpp"
#include "abcgOpenGL.hpp"

class Space {
public:
    Space();
    ~Space();

    void show(const Camera& camera);
    float size() { return m_Size_; }

    static void setup();
private:
    float m_Size_ = 10.f;
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
