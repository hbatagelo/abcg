#ifndef LIGHT_HPP_
#define LIGHT_HPP_

#include <iostream>
#include <glm/gtc/random.hpp>
#include "abcgOpenGL.hpp"
#include "camera.hpp"
#include "shader.hpp"

struct LightProperty {
    //properties
    glm::vec3 ambient = {1.f, 1.f, 1.f};
    glm::vec3 diffuse = {1.f, 1.f, 1.f};
    glm::vec3 specular = {1.f, 1.f, 1.f};

    //shader uniform location
    GLint LocationAmbient = 0;
    GLint LocationDiffuse = 0;
    GLint LocationSpecular = 0;

    void showUI() {
        ImGui::ColorEdit3("Ambient", &ambient[0]);
        ImGui::ColorEdit3("Diffuse", &diffuse[0]);
        ImGui::ColorEdit3("Specular", &specular[0]);
    }
};

struct LightAttenuation {
    //properties
    float constant = 1.f;
    float linear = 0.027f;
    float quadratic = 0.0028f;

    //shader uniform location
    GLint LocationConstant;
    GLint LocationLinear;
    GLint LocationQuadractic;

    void showUI() {
        ImGui::SliderFloat("Contant", &constant, 0.f, 2.f);
        ImGui::SliderFloat("Linear", &linear, 0.f, 0.07f, "%.4f");
        ImGui::SliderFloat("Quadratic", &quadratic, 0.f, 0.017f, "%.5f");
    }
};

struct DirLight : public LightProperty {
    //properties
    glm::vec3 direction;
    float strength = 0.25f;

    //shader uniform location
    GLint LocationDirection;
    GLint LocationStrength;

    DirLight() {
        direction = glm::normalize(m_InputDirection_);
    }

    void showUI(int id) {
        ImGui::PushID(id);
            if (!ImGui::CollapsingHeader("Directional Light")) {
                return;
            }
            if (ImGui::SliderFloat3("Direction", &m_InputDirection_[0], -1.f, 1.f)) {
                direction = glm::normalize(m_InputDirection_);
            }
            ImGui::SliderFloat("Strength", &strength, 0.f, 1.f);
            LightProperty::showUI();
        ImGui::PopID();
    }

private:
    glm::vec3 m_InputDirection_ = {-1.f, -1.f, 0.f};
};

struct PointLight : public LightProperty, LightAttenuation {
    //properties
    glm::vec3 position;
    //shader uniform location
    GLint LocationPosition;

    PointLight(float spaceSize) {
        m_SpaceSize_ = spaceSize;
        position = glm::ballRand(m_SpaceSize_);
        ambient = glm::vec3(glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f));
        diffuse = ambient;
        specular = ambient;
        calcModel();
    }

    void showUI(int id) {
        ImGui::PushID(id);
            std::string name = "Point Light #" + std::to_string(id);
            if (!ImGui::CollapsingHeader(name.c_str())) {
                return;
            }
            if (ImGui::SliderFloat3("Position", &position[0], -1.f*m_SpaceSize_, m_SpaceSize_)) {
                calcModel();
            }
            LightProperty::showUI();
            LightAttenuation::showUI();
        ImGui::PopID();
    }

    static void setup(const Shader& shader);
    void show(const Camera& camera, const Shader& shader);
private:
    void calcModel() {
        m_Model_ = glm::translate(glm::mat4(1.f), position);
        m_Model_ = glm::scale(m_Model_, glm::vec3(0.5f));
    }

    glm::mat4 m_Model_;
    float m_SpaceSize_;

    //OpenGL stuff
    static unsigned int s_EBOSize_;
    static GLuint s_VAO_;
    static GLuint s_VBO_;
    static GLuint s_EBO_;
};

struct SpotLight : public LightProperty, LightAttenuation {
    //properties
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    //shader uniform location
    GLint LocationPosition;
    GLint LocationDirection;
    GLint LocationCutOff;
    GLint LocationOuterCutOff;

    SpotLight(float spaceSize) {
        m_SpaceSize_ = spaceSize;
        position = glm::ballRand(spaceSize);
        m_XRot_ = glm::linearRand(-180.f, 180.f);
        m_YRot_ = glm::linearRand(-180.f, 180.f);
        m_ZRot_ = glm::linearRand(-180.f, 180.f);

        ambient = glm::vec3(glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f));
        diffuse = ambient;
        specular = ambient;

        linear = 0.014f;
        quadratic = 0.0007f;

        cutOff = glm::cos(glm::radians(m_CutoffAngle_));
        outerCutOff = glm::cos(glm::radians(m_OuterCutoffAngle_));
        calcModel();
    }

    void showUI(int id) {
        ImGui::PushID(id);
            std::string name = "Spot Light #" + std::to_string(id);
            if (!ImGui::CollapsingHeader(name.c_str())) {
                return;
            }

            if (ImGui::SliderFloat3("Position", &position[0], -1.f*m_SpaceSize_, m_SpaceSize_)) {
                calcModel();
            }
            if (ImGui::SliderFloat("X Rotation", &m_XRot_, -180.f, 180.f)) {
                calcModel();
            }
            if (ImGui::SliderFloat("Y Rotation", &m_YRot_, -180.f, 180.f)) {
                calcModel();
            }
            if (ImGui::SliderFloat("Z Rotation", &m_ZRot_, -180.f, 180.f)) {
                calcModel();
            }

            if (ImGui::SliderFloat("Cutoff Angle", &m_CutoffAngle_, 0.f, m_OuterCutoffAngle_)) {
                cutOff = glm::cos(glm::radians(m_CutoffAngle_));
            }

            if (ImGui::SliderFloat("Outer Cutoff Angle", &m_OuterCutoffAngle_, m_CutoffAngle_, 45.f)) {
                outerCutOff = glm::cos(glm::radians(m_OuterCutoffAngle_));
            }

            LightProperty::showUI();
            LightAttenuation::showUI();
        ImGui::PopID();
    }

    static void setup(const Shader& shader);
    void show(const Camera& camera, const Shader& shader);
private:
    void calcModel() {
        m_Model_ = glm::translate(glm::mat4(1.f), position);
        m_Model_ = glm::rotate(m_Model_, glm::radians(m_XRot_), glm::vec3(1.f, 0.f, 0.f));
        m_Model_ = glm::rotate(m_Model_, glm::radians(m_YRot_), glm::vec3(0.f, 1.f, 0.f));
        m_Model_ = glm::rotate(m_Model_, glm::radians(m_ZRot_), glm::vec3(0.f, 0.f, 1.f));
        m_Model_ = glm::scale(m_Model_, glm::vec3(0.5f));

        glm::mat4 rot = glm::rotate(glm::mat4(1.f), glm::radians(m_XRot_), glm::vec3(1.f, 0.f, 0.f));
        rot = glm::rotate(rot, glm::radians(m_YRot_), glm::vec3(0.f, 1.f, 0.f));
        rot = glm::rotate(rot, glm::radians(m_ZRot_), glm::vec3(0.f, 0.f, 1.f));
        direction = glm::vec3(rot * glm::vec4(0.f, 1.f, 0.f, 1.f));
    }

    float m_CutoffAngle_ = 23.f;
    float m_OuterCutoffAngle_ = 24.09f;
    float m_XRot_ = 0.f;
    float m_YRot_ = 0.f;
    float m_ZRot_ = 0.f;
    glm::mat4 m_Model_;
    float m_SpaceSize_;

    static unsigned int s_EBOSize_;
    static GLuint s_VAO_;
    static GLuint s_VBO_;
    static GLuint s_EBO_;
};

#endif
