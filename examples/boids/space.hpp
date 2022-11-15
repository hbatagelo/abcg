#ifndef SPACE_HPP_
#define SPACE_HPP_

#include "camera.hpp"
#include "abcgOpenGL.hpp"

class Space {
public:
    //NOTE: The space goes from -m_Size_ to m_Size_

    Space();
    ~Space();

    //Render
    void show(const Camera& camera);

    //Get the space size
    float size() { return m_Size_; }

    //Generate the OpenGL Buffers
    static void setup();
private:
    float m_Size_ = 100.f;
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
