#ifndef LIGHT_HPP_
#define LIGHT_HPP_

#include <iostream>
#include <glm/gtc/random.hpp>
#include "abcgOpenGL.hpp"
#include "camera.hpp"
#include "shader.hpp"

struct LightProperty {
    glm::vec3 ambient = {1.f, 1.f, 1.f};
    glm::vec3 diffuse = {1.f, 1.f, 1.f};
    glm::vec3 specular = {1.f, 1.f, 1.f};

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
    float constant = 1.f;
    float linear = 0.027f;
    float quadratic = 0.0028f;

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
    DirLight() {
        direction = glm::normalize(m_InputDirection_);
        strength = 0.25f;
    }

    glm::vec3 direction;
    GLint LocationDirection;

    float strength;
    GLint LocationStrength;

    void showUI() {
        ImGui::PushID(1);
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
    PointLight(int id) : position(glm::ballRand(100.f)), m_ID_(id) {
        ambient = glm::vec3(glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f));
        // ambient = glm::vec3(1.f, 1.f, 1.f);
        diffuse = ambient;
        specular = ambient;
    }

    glm::vec3 position;
    GLint LocationPosition;

    void showUI() {
        ImGui::PushID(m_ID_);
        std::string name = "Point Light #" + std::to_string(m_ID_);
        if (!ImGui::CollapsingHeader(name.c_str())) {
            return;
        }
        ImGui::SliderFloat3("Position", &position[0], -100.f, 100.f);
        LightProperty::showUI();
        LightAttenuation::showUI();
        ImGui::PopID();
    }

    static void setup(const Shader& shader);
    void show(const Camera& camera, const Shader& shader);
private:
    int m_ID_;

    static unsigned int s_EBOSize_;
    static GLuint s_VAO_;
    static GLuint s_VBO_;
    static GLuint s_EBO_;
};

#endif
