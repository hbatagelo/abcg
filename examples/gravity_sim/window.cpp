#include "window.hpp"
#include <iostream>
#include <thread>

void Window::onCreate() {
	std::random_device rd;
	m_Gen_ = std::mt19937(rd());
	m_Dis_ = std::uniform_real_distribution<float>(0.f, 1.01f);
	m_Next_color_ = glm::vec3(m_Dis_(m_Gen_), m_Dis_(m_Gen_), m_Dis_(m_Gen_));

	m_Mass_ = std::pow(2, m_Mass_power_);
}

void Window::onPaintUI() {
    const auto width = gsl::narrow<float>(getWindowSettings().width);
    const auto height = gsl::narrow<float>(getWindowSettings().height);
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    {
		ImGui::Begin("Gravity_Sim", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		auto draw_list = ImGui::GetWindowDrawList();
		auto& io = ImGui::GetIO();

		ImGui::Text("FPS: %.2f", 1.f/io.DeltaTime);
		ImGui::Text("Particles: %u", (unsigned int)m_Particles_.size());

		ImGui::SliderFloat("Gravity", &m_Gravity_, 0, 150);
		ImGui::SameLine();
		if (ImGui::Button("Clear Particles")) {
			m_Particles_.clear();
		}

		//mass slider and the mass it self
		if (ImGui::SliderInt("Mass power", &m_Mass_power_, 2, 20)) {
			m_Mass_ = std::pow(2, m_Mass_power_);
		}
		ImGui::SameLine();
		ImGui::Text("| Mass: %.1f", m_Mass_);

		ImGui::ColorEdit3("Particle Color", &m_Next_color_.r);

		//gets the window size
		auto canvas_sz = ImGui::GetContentRegionAvail();
		//enforces minimum xy size
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;

		//top left
		auto canvas_p0 = ImGui::GetCursorScreenPos();
		//bottom right
		ImVec2 canvas_p1(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		//draw the canvas
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
		//draw the border
		draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

		//makes the canvas a invisible button so we can check for clicks in it
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered

		// Add first and second point of the shooting line
		if (is_hovered && !m_Adding_particle && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			m_Mouse_pos_.push_back(glm::vec2(io.MousePos.x, io.MousePos.y));
			m_Mouse_pos_.push_back(glm::vec2(io.MousePos.x, io.MousePos.y));
			m_Adding_particle = true;
		}
		if (m_Adding_particle)
		{
			//changes the second point of the shooting line to the current mouse position
			m_Mouse_pos_.back() = glm::vec2(io.MousePos.x, io.MousePos.y);
			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				m_Adding_particle = false;

				//creates a new particle
				auto vel = m_Mouse_pos_[0] - m_Mouse_pos_[1];
				m_Particles_.emplace_back(m_Mouse_pos_[1], vel, m_Mass_, m_Next_color_);

				//generates the next color
				m_Next_color_ = glm::vec3(m_Dis_(m_Gen_), m_Dis_(m_Gen_), m_Dis_(m_Gen_));

				//removes the shooting line
				m_Mouse_pos_.clear();
			}
		}

		//optimization/fix the problem of particles outside the cavas area
		draw_list->PushClipRect(canvas_p0, canvas_p1, true);

		//draws the shooting line
		if (!m_Mouse_pos_.empty())
			draw_list->AddLine(ImVec2(m_Mouse_pos_[0].x, m_Mouse_pos_[0].y), ImVec2(m_Mouse_pos_[1].x, m_Mouse_pos_[1].y), IM_COL32(255, 255, 255, 255), 2.0f);
		
		//reset the force of all particles before doing any calculation
		for (auto& particle : m_Particles_) {
			particle.resetForce();
		}

		//iterates over every two particles
		for (uint32_t i = 0; i < m_Particles_.size(); i++) {
			auto& particle = m_Particles_[i];
			//absorbed don't need to be checked again
			if (particle.was_absorbed())
				continue;
			
			for (uint32_t j = i + 1; j < m_Particles_.size(); j++) {
				auto& other_particle = m_Particles_[j];
				//absorbed don't need to be checked again
				if (other_particle.was_absorbed())
					continue;
				particle.check_for_absorption(other_particle);
			}
		}

		//deletes particles that were absorbed
		m_Particles_.erase(
			std::remove_if(m_Particles_.begin(), m_Particles_.end(), [](const Particle& p) {return p.was_absorbed();}), 
			m_Particles_.end()
		);

		//iterates over every two particles
		for (uint32_t i = 0; i < m_Particles_.size(); i++) {
			//draws the particle
			auto& particle = m_Particles_[i];
			const auto& pos = particle.getPos();
			const auto& col = particle.getColor();
			const ImU32 color = ImColor(ImVec4(col.r, col.g, col.b, 1.0f));
			draw_list->AddCircleFilled(ImVec2(pos.x, pos.y), particle.getRadius(), color, 0);

			for (uint32_t j = i + 1; j < m_Particles_.size(); j++) {
				auto& other_particle = m_Particles_[j];
				//calculates the interaction between two particles
				particle.interact(other_particle, m_Gravity_);
			}

			//updates the particle position
			particle.updatePos();
		}

		draw_list->PopClipRect();

		ImGui::End();
	}
}
