#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>

#include "abcg.hpp"

void OpenGLWindow::initializeGL() {
  // Esse Shader usa duas variáveis novas: translation e scale, que serão responsáveis pelas diferentes posições e tamanhos dos polígonos
  const auto *vertexShader{R"gl(
    #version 410

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    uniform vec2 translation;
    uniform float scale;

    out vec4 fragColor;

    void main() {
      vec2 newPosition = inPosition * scale + translation;
      gl_Position = vec4(newPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};

  const auto *fragmentShader{R"gl(
    #version 410

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};

  // Create shader program
  m_program = createProgramFromString(vertexShader, fragmentShader);

  // Clear window
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());
}

void OpenGLWindow::paintGL() {
  // Check whether to render the next polygon
  // Se o polígono tiver side desenhado a menos do que m_delay, então a paintGL não será executada
  if (m_elapsedTimer.elapsed() < m_delay / 1000.0) return;
  m_elapsedTimer.restart();

  // Create a regular polygon with a number of sides in the range [3,20]
  std::uniform_int_distribution<int> intDist(3, 20);
  const auto sides{intDist(m_randomEngine)};
  setupModel(sides);

  // Cria a Viewport ocupando toda a janela
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  // Choose a random xy position from (-1,-1) to (1,1)
  // Cria a distribuição uniforme de -1 a 1
  std::uniform_real_distribution<float> rd1(-1.0f, 1.0f);
  // Usa o gerador aletório para pegar 2 valores da distrbuição e criar o vetor de translação
  const glm::vec2 translation{rd1(m_randomEngine), rd1(m_randomEngine)};
  // Pergunta ao programa de Shader em que posição está a variável translation
  const GLint translationLocation{
      abcg::glGetUniformLocation(m_program, "translation")};
  // Associa a posição da variável translation no shader os valores contidos em translation
  // 2fv = vetor de 2 floats
  // 1 Significa que 1 vec2 será copiado para essa posição
  // passamos &translation.x pois queremos indicar que a posição de x é o primeiro valor desse vetor
  abcg::glUniform2fv(translationLocation, 1, &translation.x); // Imagino ser igual a glUniform2f(translationLocation, translation.x, translation.y)

  // Choose a random scale factor (1% to 25%)
  // Escolhemos um numeroaleatorio entre 0.1 e 0.25. e salvamos em scale
  // Salvamos scale na posição do buffer que o shader vai ler
  std::uniform_real_distribution<float> rd2(0.01f, 0.25f);
  const auto scale{rd2(m_randomEngine)};
  const GLint scaleLocation{abcg::glGetUniformLocation(m_program, "scale")};
  abcg::glUniform1f(scaleLocation, scale);

  // Render
  abcg::glBindVertexArray(m_vao);
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    // cria um widget de 200x72 no canto inferior direito da tela
    const auto widgetSize{ImVec2(200, 72)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5,
                                   m_viewportHeight - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);

    // Especifica que o Widget não tem títulonem pode ser redimensionado ou minimizado
    const auto windowFlags{ImGuiWindowFlags_NoResize |
                           ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin(" ", nullptr, windowFlags);


    ImGui::PushItemWidth(140);
    // Cria o slider Delay, que vai de 0 a 200 e salva o valor em m_delay, mostrando o valor do delay em ms
    ImGui::SliderInt("Delay", &m_delay, 0, 200, "%d ms");
    ImGui::PopItemWidth();


    if (ImGui::Button("Clear window", ImVec2(-1, 30))) {
      abcg::glClear(GL_COLOR_BUFFER_BIT);
    }

    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_vboPositions);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void OpenGLWindow::setupModel(int sides) {
  // Release previous resources, if any
  abcg::glDeleteBuffers(1, &m_vboPositions);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);

  // Select random colors for the radial gradient
  std::uniform_real_distribution<float> rd(0.0f, 1.0f);
  const glm::vec3 color1{rd(m_randomEngine), rd(m_randomEngine),
                         rd(m_randomEngine)};
  const glm::vec3 color2{rd(m_randomEngine), rd(m_randomEngine),
                         rd(m_randomEngine)};

  // Minimum number of sides is 3
  // Desnecessário pois sides já foi definido como algo entre 3 e 20
  sides = std::max(3, sides);

  // Variáveis que irão guardar as posições e cores dos vértices.
  // São definidas dessa forma pois seus tamanhos podem ser arbitrários, então conforme definirmos novos vértices eles são adicionados ao vetor (igual append no Python)
  std::vector<glm::vec2> positions(0);
  std::vector<glm::vec3> colors(0);

  // Polygon center
  // Define o position como 0 0 (Origem) e a cor como a color1 (randomica)
  positions.emplace_back(0, 0);
  colors.push_back(color1);

  // Border vertices
  const auto step{M_PI * 2 / sides};
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
    colors.push_back(color2);
  }

  // Duplicate second vertex
  positions.push_back(positions.at(1));
  colors.push_back(color2);

  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_vboPositions);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of colors
  abcg::glGenBuffers(1, &m_vboColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                     colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const auto positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  const auto colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  // Vamos ensinar ao VAO como os dados irão funcionar

  // Nós usaremos o atributo de posição do vértice (achamos onde o Shader guarda ele na linha 179)
  abcg::glEnableVertexAttribArray(positionAttribute);
  // Informamos que existe uma vbo com esses dados de posição (m_vboPositions)
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  // a partir de positionAttribute temos uma lista de Bytes (Buffer)
  // Apartir dessa posição pegamos 2 valores (x, y)
  // Esses valores são do tipo Float
  // Indicamos que as coordenados não precisam ser normalizadas
  // Indicamos que existem 0 bytes de atributos entre o final da posição de um vértice e o início da posição do próximo vértice
  // Indicamos que existe nullptr (Zero) Bytes antes da primeira posição
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}