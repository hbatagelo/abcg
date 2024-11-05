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

  // Create shader program
  m_program = abcg::createOpenGLProgram(
      {{.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
       {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  // Clear window
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);



  calculateGraphPoints();
}
void Window::onPaint() {
  // Limpar o buffer de cor (limpar a tela)
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // Atualiza o modelo com os pontos do gráfico
  setupModel();

  // Set the viewport
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // Start using the shader program
  abcg::glUseProgram(m_program);
  // Start using VAO
  abcg::glBindVertexArray(m_VAO);

  // Draw the graph points
  abcg::glDrawArrays(GL_LINE_STRIP, 0, m_graphPoints.size());

  // End using VAO
  abcg::glBindVertexArray(0);
  // End using the shader program
  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();  // Mantém a funcionalidade padrão do OpenGL

  // Nossa interface personalizada começa aqui
  ImGui::Begin("Escolha os parâmetros de investimento:");

  // Controle para o investimento inicial (P)
  if (ImGui::SliderFloat("Investimento Inicial (P)", &m_P, 100.0f, 10000.0f, "%.1f")) {
    // Recalcula os pontos do gráfico quando o valor de P mudar
    calculateGraphPoints();
  }

  // Controle para a contribuição mensal (PMT)
  if (ImGui::SliderFloat("Contribuição Mensal (PMT)", &m_PMT, 50.0f, 1000.0f, "%.1f")) {
    // Recalcula os pontos do gráfico quando o valor de PMT mudar
    calculateGraphPoints();
  }

  // Controle para a taxa de juros mensal (i)
  if (ImGui::SliderFloat("Taxa de Juros Mensal (i)", &m_i, 0.001f, 0.1f, "%.4f")) {
    // Recalcula os pontos do gráfico quando o valor de i mudar
    calculateGraphPoints();
  }

  // Controle para o número máximo de meses (nMax)
  if (ImGui::SliderInt("Número de Meses (nMax)", &m_nMax, 12, 240)) {
    // Recalcula os pontos do gráfico quando o número de meses mudar
    calculateGraphPoints();
  }

  // Exibir os valores atuais para o usuário
  ImGui::Text("Investimento Inicial: %.1f", m_P);
  ImGui::Text("Contribuição Mensal: %.1f", m_PMT);
  ImGui::Text("Taxa de Juros Mensal: %.4f", m_i);
  ImGui::Text("Número de Meses: %d", m_nMax);

  // Finaliza a criação da janela do ImGui
  ImGui::End();
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  // Release shader program, VBO and VAO
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOVertices);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}
void Window::setupModel() {
  // Release previous VBO and VAO
  abcg::glDeleteBuffers(1, &m_VBOVertices);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Generate a new VBO and get the associated ID
  abcg::glGenBuffers(1, &m_VBOVertices);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);

  // Upload data to VBO
  abcg::glBufferData(GL_ARRAY_BUFFER, m_graphPoints.size() * sizeof(glm::vec2), m_graphPoints.data(), GL_STATIC_DRAW);

  // Get location of attributes in the program
  auto const positionAttribute = abcg::glGetAttribLocation(m_program, "inPosition");

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::calculateGraphPoints() {
    m_graphPoints.clear();  // Limpa os pontos anteriores

    // Parâmetros financeiros
    float maxY = m_P * pow(1 + m_i, m_nMax) + m_PMT * (pow(1 + m_i, m_nMax) - 1) / m_i;

    // Normaliza os valores para a faixa [-1, 1]
    for (int n = 1; n <= m_nMax; ++n) {
      // Calculando o valor acumulado com juros compostos
      float totalAmount = m_P * pow(1 + m_i, n) + m_PMT * (pow(1 + m_i, n) - 1) / m_i;

      // Normaliza X para a faixa [-1, 1]
      float normalizedX = (n / static_cast<float>(m_nMax)) * 2.0f - 1.0f;

      // Normaliza Y para a faixa [-1, 1]
      float normalizedY = (totalAmount / maxY) * 2.0f - 1.0f;

      // Adiciona o ponto ao vetor de pontos do gráfico
      m_graphPoints.push_back(glm::vec2(normalizedX, normalizedY));
    }
}