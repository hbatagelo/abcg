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

    void computeViewMatrix();
    void computeProjectionMatrix(glm::vec2 const &size);

    const glm::mat4& getViewMatrix() const { return m_ViewMatrix_; }
    const glm::mat4& getProjMatrix() const { return m_ProjMatrix_; }
    
    void showUI();

private:
    // glm::vec3 m_Eye_{150.f, 150.f, 180.f};    // Camera position
    glm::vec3 m_Eye_{55.f, 154.f, 187.f};    // Camera position
    // glm::vec3 m_Eye_{72.f, 64.f, 97.f};    // Camera position
    
    glm::vec3 m_At_{0.0f, 0.0f, 0.0f};  // Look-at point
    glm::vec3 m_Up_{0.0f, 1.0f, 0.0f};  // "up" direction

    // Matrix to change from world space to camera space
    glm::mat4 m_ViewMatrix_;

    // Matrix to change from camera space to clip space
    glm::mat4 m_ProjMatrix_;
};

#endif
