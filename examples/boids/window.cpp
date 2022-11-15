#include "window.hpp"
#include <iostream>
#include <glm/gtc/random.hpp>

void Window::onCreate() {
	abcg::glEnable(GL_DEPTH_TEST);
	abcg::glDepthFunc(GL_LESS);

	abcg::glEnable(GL_BLEND);
	abcg::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//Calling computeViewMatrix at least once before the render beginds
	m_Camera_.computeViewMatrix();
	
	//Generate the OpenGL buffers
	Boid::setup();
	Space::setup();

	createBoids();
}

void Window::createBoids() {
	//Create m_NumberOfBoids_ of boid at random positions and with random velocities
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

		Boid::showUI();
		m_Camera_.showUI();
		ImGui::End();
	}
}

void Window::onPaint() {
	//Get the frame time so we can
	//make the simulation independent of the framerate
	auto dt = m_Timer_.elapsed();
	m_Timer_.restart();

	abcg::glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& b : m_Boids_) {
		b.simulate(m_Boids_);
		b.update(dt);
		b.checkEdge(m_Space_.size());
		b.show(m_Camera_);
	}

	m_Space_.show(m_Camera_);
}

void Window::onResize(glm::ivec2 const &size) {
	m_Camera_.computeProjectionMatrix(size);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_Camera_.onUpdate(deltaTime);
  m_Camera_.computeViewMatrix();
}

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_Camera_.setDollySpeed(100.0f);
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_Camera_.setDollySpeed(-100.0f);
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_Camera_.setPanSpeed(-1.0f);
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_Camera_.setPanSpeed(1.0f);
    if (event.key.keysym.sym == SDLK_q)
      m_Camera_.setTruckSpeed(-100.0f);
    if (event.key.keysym.sym == SDLK_e)
      m_Camera_.setTruckSpeed(100.0f);
  }
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_Camera_.getDollySpeed() > 0)
      m_Camera_.setDollySpeed(0.0f);
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_Camera_.getDollySpeed() < 0)
      m_Camera_.setDollySpeed(0.0f);
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        m_Camera_.getPanSpeed() < 0)
      m_Camera_.setPanSpeed(0.0f);
    if ((event.key.keysym.sym == SDLK_RIGHT ||
         event.key.keysym.sym == SDLK_d) &&
        m_Camera_.getPanSpeed() > 0)
      m_Camera_.setPanSpeed(0.0f);
    if (event.key.keysym.sym == SDLK_q && m_Camera_.getTruckSpeed() < 0)
      m_Camera_.setTruckSpeed(0.0f);
    if (event.key.keysym.sym == SDLK_e && m_Camera_.getTruckSpeed() > 0)
      m_Camera_.setTruckSpeed(0.0f);
  }
}