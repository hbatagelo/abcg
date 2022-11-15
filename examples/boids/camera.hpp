#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera {
public:
    void computeViewMatrix();
    void computeProjectionMatrix(glm::vec2 const &size);

    // void dolly(float speed);
    // void truck(float speed);
    // void pan(float speed);

    glm::mat4 const &getViewMatrix() const { return m_viewMatrix; }
    glm::mat4 const &getProjMatrix() const { return m_projMatrix; }

private:
    glm::vec3 m_eye{15.f, 15.f, 18.f};    // Camera position
    glm::vec3 m_at{0.0f, 0.0f, 0.0f};  // Look-at point
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};  // "up" direction

    // Matrix to change from world space to camera space
    glm::mat4 m_viewMatrix;

    // Matrix to change from camera space to clip space
    glm::mat4 m_projMatrix;
};

#endif
