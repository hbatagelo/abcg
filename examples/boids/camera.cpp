#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// void Camera::computeProjectionMatrix(const glm::vec2& size) {
void Camera::computeProjectionMatrix() {
    const auto aspect = m_Size_.x / m_Size_.y;
    m_ProjMatrix_ = glm::perspective(glm::radians(m_Fov_), aspect, m_ZNear_, m_ZFar_);
}

void Camera::computeViewMatrix() {
    m_ViewMatrix_ = glm::lookAt(m_Eye_, m_Eye_ + m_Front_, m_Up_);
}

void Camera::showUI() {
    if (!ImGui::CollapsingHeader("Camera"))
        return;

    ImGui::SliderFloat("Speed", &m_Speed_, 0.f, 1000.f);
    if (ImGui::SliderFloat("FOV", &m_Fov_, 1.f, 359.f)) {
        computeProjectionMatrix();
    }
    if (ImGui::SliderFloat("Z Near", &m_ZNear_, 0.1f, 500.f)) {
        computeProjectionMatrix();
    }
    if (ImGui::SliderFloat("Z Far", &m_ZFar_, 50.f, 1000.f)) {
        computeProjectionMatrix();
    }
}

void Camera::onEvent(const SDL_Event& event, float dt) {
    const float cameraSpeed = m_Speed_ * dt;
    if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
			m_Eye_ += cameraSpeed * m_Front_;
		if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
			m_Eye_ -= cameraSpeed * m_Front_;
		if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
			m_Eye_ -= glm::normalize(glm::cross(m_Front_, m_Up_)) * cameraSpeed;
		if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
			m_Eye_ += glm::normalize(glm::cross(m_Front_, m_Up_)) * cameraSpeed;
	}

    if (event.type == SDL_MOUSEMOTION && event.button.button == SDL_BUTTON_LEFT) {
        auto x = (float)event.button.x;
        auto y = (float)event.button.y;

        if (m_FirstEvent_) {
            m_FirstEvent_  = false;
            m_LastX_ = x;
            m_LastY_ = y;
            return;
        }
        auto xOffset = x - m_LastX_;
        auto yOffset = m_LastY_ - y;
        m_LastX_ = x;
        m_LastY_ = y;

        float sensitivity = 0.1f;
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        m_Yaw_   += xOffset;
        m_Pitch_ += yOffset;

        if(m_Pitch_ > 89.0f)
            m_Pitch_ = 89.0f;
        if(m_Pitch_ < -89.0f)
            m_Pitch_ = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(m_Yaw_)) * cos(glm::radians(m_Pitch_));
        direction.y = sin(glm::radians(m_Pitch_));
        direction.z = sin(glm::radians(m_Yaw_)) * cos(glm::radians(m_Pitch_));
        m_Front_ = glm::normalize(direction);
    }

    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        m_FirstEvent_  = true;
    }

    computeViewMatrix();
}
