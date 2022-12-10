#include "window.hpp"
#include <glm/gtc/random.hpp>
#include <iostream>

void Window::onCreate() {
	abcg::glEnable(GL_DEPTH_TEST);
	abcg::glDepthFunc(GL_LESS);

	abcg::glEnable(GL_BLEND);
	abcg::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Calling computeViewMatrix at least once before the render beginds
	m_Camera_.computeViewMatrix();

	// Generate the OpenGL buffers
	m_Shader_ = std::make_unique<Shader>();
	PointLight::setup(*m_Shader_);
	SpotLight::setup(*m_Shader_);
	Space::setup(*m_Shader_);
	Boid::setup(m_Space_);

	createBoids();
}

void Window::createBoids() {
	// Create m_NumberOfBoids_ of boid at random positions and with random
	// velocities
	m_Boids_.clear();
	m_Boids_.reserve(m_NumberOfBoids_);
	for (uint32_t i = 0; i < (uint32_t)m_NumberOfBoids_; i++) {
		auto pos = glm::ballRand(m_Space_.size());
		auto vel = glm::ballRand(Boid::maxVel());
		m_Boids_.emplace_back(pos, vel);
	}
}

void Window::onPaintUI() {
	abcg::OpenGLWindow::onPaintUI();
	{
		ImGui::Begin("Boid", nullptr);
		if (ImGui::InputInt("Number of Boids", &m_NumberOfBoids_)) {
			createBoids();
		}

		int id = 0;
		Boid::showUI(id);
		id++;

		m_Camera_.showUI();

		if (ImGui::CollapsingHeader("Lights")) {
			ImGui::Indent();

			ImGui::Checkbox("Use Direct Light", &m_UseDirLight_);
			ImGui::Checkbox("Use Point Light", &m_UsePointLight_);
			ImGui::Checkbox("Use Spot Light", &m_UseSpotLight_);

			m_Space_.getDirLight().showUI(id);
			id++;

			if (ImGui::CollapsingHeader("Point Lights")) {
				ImGui::Indent();
				for (auto& light : m_Space_.getPointLights()) {
					light.showUI(id);
					id++;
				}
				ImGui::Unindent();
			}			
			
			if (ImGui::CollapsingHeader("Spot Lights")) {
				ImGui::Indent();
				for (auto& light : m_Space_.getSpotLights()) {
					light.showUI(id);
					id++;
				}
				ImGui::Unindent();
			}

			ImGui::Unindent();
		}
		ImGui::End();
	}
}

void Window::onPaint() {
	// Get the frame time so we can
	// make the simulation independent of the framerate
	auto dt = m_Timer_.elapsed();
	m_Timer_.restart();

	abcg::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_UsePointLight_) {
		for (auto& light : m_Space_.getPointLights()) {
			light.show(m_Camera_, *m_Shader_);
		}
	}

	if (m_UseSpotLight_) {
		for (auto& light : m_Space_.getSpotLights()) {
			light.show(m_Camera_, *m_Shader_);
		}
	}

	for (auto &b : m_Boids_) {
		b.simulate(m_Boids_);
		b.update(dt);
		b.checkEdge(m_Space_.size());
		b.show(m_Camera_, m_Space_, m_UseDirLight_, m_UsePointLight_, m_UseSpotLight_);
	}

	m_Space_.show(m_Camera_, *m_Shader_);
}

void Window::onResize(const glm::ivec2& size) {
	abcg::glViewport(0, 0, size.x, size.y);
	m_Camera_.setSize(size);
}

void Window::onEvent(const SDL_Event& event) {
	auto dt = m_Timer_.elapsed();
	m_Camera_.onEvent(event, dt);
}
