#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "abcgOpenGL.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera {
public:
    /*
        TODO:
            * Adicionar movimentacao com teclado e mouse
                Dica: https://learnopengl.com/Getting-started/Camera
                Dica: Dar uma olhada no que o professor fez
                Dica: Adicionar update nesta classe que é chamado no onUpdate() da classe Window
            * Adicionar mais alguns sliders no render da UI, como FOV, zNear, zFar
                Dica: Adicionar algumas variaveis e mexer no showUI
                Dica: Lembrar de chamar computeViewMatrix e computeProjectionMatrix
    */
    void dolly(float speed);
    void truck(float speed);
    void pan(float speed);
    // void onUpdate(float deltaTime);
    void onEvent(const SDL_Event& event, float dt);

    void computeViewMatrix();
    void computeProjectionMatrix(glm::vec2 const &size);

    const glm::mat4& getViewMatrix() const { return m_ViewMatrix_; }
    const glm::mat4& getProjMatrix() const { return m_ProjMatrix_; }
    
    void showUI();

    void setDollySpeed(float value);
    void setPanSpeed(float value);
    void setTruckSpeed(float value);

    float getDollySpeed();
    float getPanSpeed();
    float getTruckSpeed();

private:

    float m_DollySpeed_{0};
    float m_TruckSpeed_{0};
    float m_PanSpeed_{0};

    // glm::vec3 m_Eye_{150.f, 150.f, 180.f};    // Camera position
    glm::vec3 m_Eye_{55.f, 154.f, 187.f};    // Camera position
    // glm::vec3 m_Eye_{72.f, 64.f, 97.f};    // Camera position
    
    // glm::vec3 m_At_{0.0f, 0.0f, 0.0f};  // Look-at point
    glm::vec3 m_At_{-0.221404f, -0.61993f, -0.752773f};  // Look-at point
    glm::vec3 m_Up_{0.0f, 1.0f, 0.0f};  // "up" direction

    // Matrix to change from world space to camera space
    glm::mat4 m_ViewMatrix_;

    // Matrix to change from camera space to clip space
    glm::mat4 m_ProjMatrix_;
};

#endif
