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


void Camera::dolly(float speed) {
  // Compute forward vector (view direction)
  auto const forward{glm::normalize(m_At_ - m_Eye_)};

  // Move eye and center forward (speed > 0) or backward (speed < 0)
  m_Eye_ += forward * speed;
  m_At_ += forward * speed;
}

void Camera::truck(float speed) {
  // Compute forward vector (view direction)
  auto const forward{glm::normalize(m_At_ - m_Eye_)};
  // Compute vector to the left
  auto const left{glm::cross(m_Up_, forward)};

  // Move eye and center to the left (speed < 0) or to the right (speed > 0)
  m_At_ -= left * speed;
  m_Eye_ -= left * speed;
}

void Camera::pan(float speed) {
  glm::mat4 transform{1.0f};

  // Rotate camera around its local y axis
  transform = glm::translate(transform, m_Eye_);
  transform = glm::rotate(transform, -speed, m_Up_);
  transform = glm::translate(transform, -m_Eye_);

  m_At_ = transform * glm::vec4(m_At_, 1.0f);
}


void Camera::onUpdate(float deltaTime) {
  dolly(m_DollySpeed_ * deltaTime);
  truck(m_TruckSpeed_ * deltaTime);
  pan(m_PanSpeed_ * deltaTime);
}


void Camera::setDollySpeed(float value){
    m_DollySpeed_ = value ;
}

void Camera::setPanSpeed(float value){
    m_PanSpeed_ = value;
}

void Camera::setTruckSpeed(float value){
    m_TruckSpeed_ = value;
}

float Camera::getDollySpeed(){
    return m_DollySpeed_;
}

float Camera::getPanSpeed(){
    return m_PanSpeed_;
}

float Camera::getTruckSpeed(){
    return m_TruckSpeed_;
}
