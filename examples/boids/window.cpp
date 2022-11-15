#include "window.hpp"
#include <iostream>
#include <thread>

void Window::onCreate() {
	std::random_device rd;
	m_Gen_ = std::mt19937(rd());
	m_Dis_ = std::uniform_real_distribution<float>(0.f, 1.01f);
	m_Camera_.computeViewMatrix();
	Boid::setup();
}

void Window::onPaintUI() {
    {
		ImGui::Begin("Boid", nullptr);
		ImGui::End();
	}
}

void Window::onPaint() {
	abcg::glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Boid b;
	b.show(m_Camera_);
}

void Window::onResize(glm::ivec2 const &size) {
	m_Camera_.computeProjectionMatrix(size);
}
