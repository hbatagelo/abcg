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


void Window::onPaintUI() {    //onde vão os widgets
  abcg::OpenGLWindow::onPaintUI();

   // Our own ImGui widgets go below
  {
    // Window begin
    ImGui::Begin("Escolha os parâmetros: ");

    // Static text
    auto const &windowSettings{getWindowSettings()};
    ImGui::Text("a: 10");

    // Slider from 0.0f to 1.0f
    static float f{};
    ImGui::SliderFloat("float", &f, 0.0f, 1000.0f);

    // ColorEdit to change the clear color
    // ImGui::ColorEdit3("clear color", m_clearColor.data());

    // More static text
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // Window end
    ImGui::End();
  }
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
}void Window::calculateGraphPoints() {
    m_graphPoints.clear(); // Limpa pontos anteriores

    // Parâmetros do investimento
    float P = 1000.0f;  // Investimento inicial
    float PMT = 200.0f; // Contribuição mensal
    float i = 0.01f;    // Taxa de juros mensal
    int nMax = 120;     // Número máximo de meses (10 anos)

    float accumulatedAmount = P; // Inicializa com o valor do investimento inicial

    // Determina o valor máximo de y para ajustar a escala (isso evita que a curva fique fora da tela)
    float maxY = P;  // Começamos com o valor inicial como maxY
    for (int n = 1; n <= nMax; ++n) {
        // Calculando o valor acumulado usando a fórmula de juros compostos
        float totalAmount = P * pow(1 + i, n) + PMT * (pow(1 + i, n) - 1) / i;

        // Verifica se o valor acumulado ultrapassou o valor máximo, para normalizar
        if (totalAmount > maxY) {
            maxY = totalAmount; // Atualiza o valor máximo de Y
        }
    }

    // Normaliza os valores para que fiquem dentro da faixa [-1, 1]
    for (int n = 1; n <= nMax; ++n) {
        // Calculando o valor acumulado usando a fórmula de juros compostos
        float totalAmount = P * pow(1 + i, n) + PMT * (pow(1 + i, n) - 1) / i;

        // Normaliza X para a faixa [-1, 1]
        float normalizedX = (n / static_cast<float>(nMax)) * 2.0f - 1.0f;

        // Normaliza Y para a faixa [-1, 1]
        float normalizedY = (totalAmount / maxY) * 2.0f - 1.0f;

        // Adiciona o ponto calculado ao vetor de pontos
        m_graphPoints.push_back(glm::vec2(normalizedX, normalizedY));
    }
}
