#include "window.hpp"
#include <iostream>

void Window::onCreate() {
	abcg::glEnable(GL_DEPTH_TEST);
	abcg::glDepthFunc(GL_LESS);

	abcg::glEnable(GL_BLEND);
	abcg::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::random_device rd;
	m_Gen_ = std::mt19937(rd());
	m_Dis_ = std::uniform_real_distribution<float>(-9.99f, 9.99f);
	
	m_Camera_.computeViewMatrix();
	
	Boid::setup();
	Space::setup();


	for (uint32_t i = 0; i < 100; i++) {
		Boid b(glm::vec3(m_Dis_(m_Gen_), m_Dis_(m_Gen_), m_Dis_(m_Gen_)));
		m_Boids_.push_back(b);
	}
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

	m_Space_.show(m_Camera_);

	for (auto& b : m_Boids_) {
		b.show(m_Camera_);
	}
}

void Window::onResize(glm::ivec2 const &size) {
	m_Camera_.computeProjectionMatrix(size);
}
