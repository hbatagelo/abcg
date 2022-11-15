#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Camera::computeProjectionMatrix(glm::vec2 const &size) {
    const auto aspect = size.x / size.y;
    m_ProjMatrix_ = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 500.0f);
}

void Camera::computeViewMatrix() {
    m_ViewMatrix_ = glm::lookAt(m_Eye_, m_At_, m_Up_);
}

void Camera::showUI() {
    if (!ImGui::CollapsingHeader("Camera"))
        return;

    if (ImGui::DragFloat3("Eye", &m_Eye_[0])) {
        computeViewMatrix();
    }
}
