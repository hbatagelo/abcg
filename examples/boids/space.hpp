#ifndef SPACE_HPP_
#define SPACE_HPP_

#include "camera.hpp"
#include "shader.hpp"
#include "light.hpp"
#include "abcgOpenGL.hpp"
#include <vector>

class Space {
public:
    //NOTE: The space goes from -m_Size_ to m_Size_

    Space();
    ~Space();

    //Render
    void show(const Camera& camera, const Shader& shader);

    //Get the space size
    float size() { return m_Size_; }

    //Generate the OpenGL Buffers
    static void setup(const Shader& shader);

    DirLight& getDirLight() { return m_DirLight_; }
    std::vector<PointLight>& getPointLights() { return m_PointLights_; }
    std::vector<SpotLight>& getSpotLights() { return m_SpotLights_; }
private:
    float m_Size_ = 100.f;
    glm::mat4 m_Model_;

    DirLight m_DirLight_;
    std::vector<PointLight> m_PointLights_ = {
        PointLight(m_Size_),
        PointLight(m_Size_),
        PointLight(m_Size_),
        PointLight(m_Size_),
        PointLight(m_Size_),
    };

    std::vector<SpotLight> m_SpotLights_ = {
        SpotLight(m_Size_),
        SpotLight(m_Size_),
        SpotLight(m_Size_),
        SpotLight(m_Size_),
        SpotLight(m_Size_),
    };

    static unsigned int s_EBOSize_;
    static GLuint s_VAO_;
    static GLuint s_VBO_;
    static GLuint s_EBO_;
};

#endif
