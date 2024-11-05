#include "window.hpp"

void Window::onCreate() {
  auto const *vertexShader{R"gl(#version 300 es
    layout(location = 0) in vec2 inPosition;

    void main() {
      gl_PointSize = 2.0;
      gl_Position = vec4(inPosition, 0, 1);
    }
  )gl"};

  auto const *fragmentShader{R"gl(#version 300 es
    precision mediump float;

    out vec4 outColor;

    void main() { outColor = vec4(1); }
  )gl"};

  // Cria o shader program
  m_program = abcg::createOpenGLProgram(
      {{.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
       {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  // Limpa a window
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  calculateGraphPoints();
}

void Window::onPaint() {
  // Limpa o buffer de cor (limpar a tela)
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // Atualiza o modelo com os pontos do gráfico
  setupModel();

  // Seta a viewport
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // Começa a usar o shader program
  abcg::glUseProgram(m_program);
  // Começa a usar a VAO
  abcg::glBindVertexArray(m_VAO);

  // Desenha os pontos do gráfico
  abcg::glDrawArrays(GL_LINE_STRIP, 0, m_graphPoints.size());

  // Termina de usar a VAO
  abcg::glBindVertexArray(0);
  // Termina de usar o shader program
  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  ImGui::Begin("Escolha os parâmetros de investimento:");

  // Controle para o investimento inicial (P)
  if (ImGui::SliderFloat("Investimento Inicial (P)", &m_P, 100.0f, 10000.0f, "%.1f")) {
    calculateGraphPoints();
  }

  // Controle para a contribuição mensal (PMT)
  if (ImGui::SliderFloat("Contribuição Mensal (PMT)", &m_PMT, 50.0f, 1000.0f, "%.1f")) {
    calculateGraphPoints();
  }

  // Controle para a taxa de juros mensal (i)
  if (ImGui::SliderFloat("Taxa de Juros Mensal (i)", &m_i, 0.001f, 0.1f, "%.4f")) {
    calculateGraphPoints();
  }

  // Controle para o número máximo de meses (nMax)
  if (ImGui::SliderInt("Número de Meses (nMax)", &m_nMax, 12, 240)) {
    calculateGraphPoints();
  }

  // Exibe os valores atuais para o usuário
  ImGui::Text("Investimento Inicial: %.1f", m_P);
  ImGui::Text("Contribuição Mensal: %.1f", m_PMT);
  ImGui::Text("Taxa de Juros Mensal: %.4f", m_i);
  ImGui::Text("Número de Meses: %d", m_nMax);

   // Exibe o total acumulado
  float totalAmount = m_P * pow(1 + m_i, m_nMax) + m_PMT * (pow(1 + m_i, m_nMax) - 1) / m_i;
  ImGui::Text("Total Acumulado: %.2f", totalAmount);

  // Finaliza a criação da janela do ImGui
  ImGui::End();
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  // Deleta o shader program, VBO e VAO
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOVertices);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::setupModel() {
  // Deleta VBO e VAO anteriores
  abcg::glDeleteBuffers(1, &m_VBOVertices);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Gera um novo VBO e pega o ID associado
  abcg::glGenBuffers(1, &m_VBOVertices);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);

  // Atualiza os dados do VBO
  abcg::glBufferData(GL_ARRAY_BUFFER, m_graphPoints.size() * sizeof(glm::vec2), m_graphPoints.data(), GL_STATIC_DRAW);

  // Pega o local dos atributos do programa
    auto const positionAttribute = abcg::glGetAttribLocation(m_program, "inPosition");

  // Cria o VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Vincula o os atributos dos vértices ao VAO atual
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::calculateGraphPoints() {
  // Limpa os pontos anteriores
  m_graphPoints.clear();

  // Parâmetros financeiros
  float maxY = m_P * pow(1 + m_i, m_nMax) + m_PMT * (pow(1 + m_i, m_nMax) - 1) / m_i;

  // Normaliza os valores para a faixa [-1, 1]
  for (int n = 1; n <= m_nMax; ++n) {
    // Calcula o valor acumulado com juros compostos
    float totalAmount = m_P * pow(1 + m_i, n) + m_PMT * (pow(1 + m_i, n) - 1) / m_i;

    // Normaliza X para a faixa [-1, 1]
    float normalizedX = (n / static_cast<float>(m_nMax)) * 2.0f - 1.0f;

    // Normaliza Y para a faixa [-1, 1]
    float normalizedY = (totalAmount / maxY) * 2.0f - 1.0f;

    // Adiciona o ponto ao vetor de pontos do gráfico
    m_graphPoints.push_back(glm::vec2(normalizedX, normalizedY));
  }
}