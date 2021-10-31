#include "openglwindow.hpp"

#include <imgui.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "abcg.hpp"

void OpenGLWindow::initializeGL() {
  // Inicializa o programa definindo as variáveis iniciais


  // Ponteiro do vertex shader
  const auto *vertexShader{R"gl(
    #version 410

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    out vec4 fragColor;

    void main() {
      gl_Position = vec4(inPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};
  // Esse Shader recebe  dois valores de entrada:
      // Posição  : (x,y) do vértice (armazenado em m_vbo_positions)
      // Cor      : (RGBA) de m_vbocolors
  // A saída é a própria cor do vértice
  // Preencher gl_Position não é obrigatório



  const auto *fragmentShader{R"gl(
    #version 410

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};
   // O Shader de fragmentos é transparente, ou seja, não altera em nada a cor do fragmento.

  // Create shader program
  m_program = createProgramFromString(vertexShader, fragmentShader);

  // Clear window
  glClearColor(0, 0, 0, 1); // Preenche mm Buffer (COLOR_BUFFER) com a cor escolhida
  glClear(GL_COLOR_BUFFER_BIT); // Pinta a tela segundo o Buffer

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);
}

void OpenGLWindow::paintGL() {
  // Chama a função SetupModel (nossa função, que cria os pontos)
  setupModel();

  // A Viewport é onde irão aparecer os elementos
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);
  abcg::glBindVertexArray(m_vao);

  // Indica que as primitivas criadas considerarão os vértices segundo o padrão GL_Triangles
  // Essa primitiva é composta dos 3 vertices a partir do vertice 0 (um triângulo)
  abcg::glDrawArrays(GL_TRIANGLES, 0, 3);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    // Define a varíável com o tamanho da área com a seleção de Cor
    auto widgetSize{ImVec2(250, 90)};
    // Define a posição desse widget para o canto inferior esquerdo com 5 pixels das bordas
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5,
                                   m_viewportHeight - widgetSize.y - 5));
    // Define o tamanho do Widget
    ImGui::SetNextWindowSize(widgetSize);
    // Retira as opções de redimensionamento e título do Widget
    auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar};
    // Desenha o Widget na tela
    ImGui::Begin(" ", nullptr, windowFlags);

    // Edit vertex colors
    // Retira a o color_picker e as dicas de ferramentas (para não poluir a tela)
    auto colorEditFlags{ImGuiColorEditFlags_NoTooltip |
                        ImGuiColorEditFlags_NoPicker};
    ImGui::PushItemWidth(215);
    // Muda as cores do vértices conforme o que está em m_vertexColors
    ImGui::ColorEdit3("v0", &m_vertexColors[0].x, colorEditFlags);
    ImGui::ColorEdit3("v1", &m_vertexColors[1].x, colorEditFlags);
    ImGui::ColorEdit3("v2", &m_vertexColors[2].x, colorEditFlags);
    ImGui::PopItemWidth();

    ImGui::End();
  }
}
// É chamada sempre que o tamanho da tela é alterado
void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}
// É chamada no final do programa para liberar os recursos gráficos
void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_vboPositions);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);
}
// É chamada toda vez dentro de paintGL
void OpenGLWindow::setupModel() {
  // Libera as VBO e as VAO da memória, para garantir que quando definirmos os novos valores não acabemos por lotar a memória da GPU
  abcg::glDeleteBuffers(1, &m_vboPositions);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);

  // Create vertex positions
  std::uniform_real_distribution<float> rd(-1.5f, 1.5f);
  std::array positions{glm::vec2(rd(m_randomEngine), rd(m_randomEngine)),
                       glm::vec2(rd(m_randomEngine), rd(m_randomEngine)),
                       glm::vec2(rd(m_randomEngine), rd(m_randomEngine))};

  // Create vertex colors
  std::vector<glm::vec4> colors(0);
  colors.emplace_back(m_vertexColors[0]);
  colors.emplace_back(m_vertexColors[1]);
  colors.emplace_back(m_vertexColors[2]);

  // Generate VBO of positions
  // Associa o espaço de um objeto de Buffer da GPU (onde estão as posições da VBO) 
  abcg::glGenBuffers(1, &m_vboPositions);
  // Associa o Buffer ao m_vboPosition, informando que isso se trata de uma Array
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  // Aloca a memória de Buffer como uma array de tamanho igual a len(positions), com os valores de positions
  // o último parâmetro indica que Buffer será estático, ou seja, depois de criado pode ser usado várias vezes sem sofrer alteração (como se fosse uma constante) 
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  // Desassocia o Buffer de m_vboPositions
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of colors
  // Funciona igual ao Buffer das Posições, só que agora com as Cores
  abcg::glGenBuffers(1, &m_vboColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4),
                     colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  // Procura no m_program da GPU os atributos (posição e cor) do vertex shader
  // Os valores serão 0 e 1 devido as linhas layout(location = x) que usamos no .hpp
  // Isso é complicado, o objeto de de vertex shader possui vários valores, que ficam alinhados como uma array. Precisamos descobrir onde dentro dessa lista começam as varíaveis que precisamos.
  // O glGetAtttribLocation informa em qual posição do objeto de shader estão as variáveis. No nosso caso definimos que esse objeto iria possuir as variáveis nas posições 0 e 1, mas se isso não tivesse sido indicado no .hpp esses números poderiam ser arbitrários.
  
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};
  GLint colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Agora podemos indicar como os dados da VBO podem ser mapeados para esses atributos.

  // Create VAO
  // O VAO é uma ponte entre os dados do vertice (VBO) e o shader (vertex shader) ele indica como que o shader deve procurar os valores corretamente dentro da VBO
  abcg::glGenVertexArrays(1, &m_vao);


  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}