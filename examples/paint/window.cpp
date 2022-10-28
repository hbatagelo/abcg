#include "window.hpp"
#include <iostream>

void Window::onCreate() {
	create_shaders();
	Rectangle::setup(m_DefaultShader_);
	Circle::setup(m_CircleShader_);
	Triangle::setup(m_DefaultShader_);
	Pentagon::setup(m_DefaultShader_);
	Star::setup(m_DefaultShader_);

	abcg::glViewport(0, 0, 1000.f, 1000.f);
}

void Window::create_shaders() {
	const auto assetsPath = abcg::Application::getAssetsPath();

	GLuint defaultProgram = abcg::createOpenGLProgram({
		{.source = assetsPath + "shader.vert", .stage = abcg::ShaderStage::Vertex},
		{.source = assetsPath + "shader.frag", .stage = abcg::ShaderStage::Fragment}
	});
	m_DefaultShader_ = Shader {
		.program = defaultProgram,
		.positionALoc = abcg::glGetAttribLocation(defaultProgram, "l_position"),
		.modelULoc = abcg::glGetUniformLocation(defaultProgram, "u_model"),
		.colorlULoc = abcg::glGetUniformLocation(defaultProgram, "u_color"),
		.applylULoc = abcg::glGetUniformLocation(defaultProgram, "u_apply"),
		.timelULoc = abcg::glGetUniformLocation(defaultProgram, "u_time"),
		.randomlULoc = abcg::glGetUniformLocation(defaultProgram, "u_random"),
		.alphalULoc = abcg::glGetUniformLocation(defaultProgram, "u_alpha"),
		.viewportlULoc = abcg::glGetUniformLocation(defaultProgram, "u_viewport"),
	};

	GLuint circleProgram = abcg::createOpenGLProgram({
		{.source = assetsPath + "shader.vert", .stage = abcg::ShaderStage::Vertex},
		{.source = assetsPath + "circle.frag", .stage = abcg::ShaderStage::Fragment}
	});
	m_CircleShader_ = Shader {
		.program = circleProgram,
		.positionALoc = abcg::glGetAttribLocation(circleProgram, "l_position"),
		.modelULoc = abcg::glGetUniformLocation(circleProgram, "u_model"),
		.colorlULoc = abcg::glGetUniformLocation(circleProgram, "u_color"),
		.applylULoc = abcg::glGetUniformLocation(circleProgram, "u_apply"),
		.timelULoc = abcg::glGetUniformLocation(circleProgram, "u_time"),
		.randomlULoc = abcg::glGetUniformLocation(circleProgram, "u_random"),
		.alphalULoc = abcg::glGetUniformLocation(circleProgram, "u_alpha"),
		.viewportlULoc = abcg::glGetUniformLocation(circleProgram, "u_viewport"),
	};
}

void Window::onEvent(SDL_Event const &event) {
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		auto x_ndc =       2.0f * (event.button.x + 0.5f) / m_Viewport_.x - 1.0f;
		auto y_ndc = -1.f*(2.0f * (event.button.y + 0.5f) / m_Viewport_.y - 1.0f);
		if (m_SelectedShape_ == SapeTypes::Rectangle) {
			m_Shapes_.push_back(std::make_unique<::Rectangle>(
				glm::vec2(x_ndc, y_ndc),
				m_Color_
			));
		} else if (m_SelectedShape_ == SapeTypes::Circle) {
			m_Shapes_.push_back(std::make_unique<::Circle>(
				glm::vec2(x_ndc, y_ndc),
				m_Color_
			));
		} else if (m_SelectedShape_ == SapeTypes::Triangle) {
			m_Shapes_.push_back(std::make_unique<::Triangle>(
				glm::vec2(x_ndc, y_ndc),
				m_Color_
			));
		} else if (m_SelectedShape_ == SapeTypes::Pentagon) {
			m_Shapes_.push_back(std::make_unique<::Pentagon>(
				glm::vec2(x_ndc, y_ndc),
				m_Color_
			));
		} else if (m_SelectedShape_ == SapeTypes::Star) {
			m_Shapes_.push_back(std::make_unique<::Star>(
				glm::vec2(x_ndc, y_ndc),
				m_Color_
			));
		}
	}

	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			m_Shapes_.clear();
		} else if (event.key.keysym.sym == SDLK_UP) {
			m_SelectedShape_--;
			if (m_SelectedShape_ < 0) {
				m_SelectedShape_ = 0;
			}
		} else if (event.key.keysym.sym == SDLK_DOWN) {
			m_SelectedShape_++;
			if (m_SelectedShape_ >= (int)m_ShapeNames_.size()) {
				m_SelectedShape_ = m_ShapeNames_.size() - 1;
			}
		}
	}
}

void Window::onPaintUI() {
	auto const appWindowWidth{gsl::narrow<float>(getWindowSettings().width)};
	auto const appWindowHeight{gsl::narrow<float>(getWindowSettings().height)};

    {
		ImGui::SetNextWindowSize(ImVec2(appWindowWidth/4.f, appWindowHeight));
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		ImGui::Begin("Paint", nullptr, ImGuiWindowFlags_NoResize);
		ImGui::ColorEdit4("Color", &m_Color_.r, ImGuiColorEditFlags_NoInputs);
		ImGui::ColorEdit3("Backgroung", &m_BakgroundColor_.r, ImGuiColorEditFlags_NoInputs);
		ImGui::Combo("Shape", &m_SelectedShape_, m_ShapeNames_.data(), m_ShapeNames_.size());

		int j = -1;
		for (uint32_t i = 0; i < m_Shapes_.size(); i++) {
			ImGui::PushID(i);
			if (!m_Shapes_[i]->draw_ui()) {
				j = i;
			}
			ImGui::PopID();
		}

		if (j != -1) {
			m_Shapes_.erase(m_Shapes_.begin()+j);
		}

		ImGui::End();
	}
}

void Window::onPaint() {
	abcg::glClearColor(m_BakgroundColor_.r, m_BakgroundColor_.g, m_BakgroundColor_.b, 1.f);
	abcg::glClear(GL_COLOR_BUFFER_BIT);
	abcg::glViewport(0, 0, m_Viewport_.x, m_Viewport_.y);


	abcg::glEnable(GL_BLEND);
	abcg::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (const auto& s : m_Shapes_) {
		s->draw(m_Viewport_);
	}
	abcg::glDisable(GL_BLEND);
}

void Window::onResize(const glm::ivec2& size) { 
	m_Viewport_ = size; 
}
