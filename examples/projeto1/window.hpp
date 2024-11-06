#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>
#include <vector> // Adicionado para armazenar os pontos do gráfico

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
public:
  // Funções de controle do ciclo de vida da janela
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  // Variáveis para os parâmetros do gráfico e do investimento
  std::default_random_engine m_randomEngine; // Mantenha esta linha
  glm::ivec2 m_viewportSize{};

  glm::vec4 m_backgroundColor{0.0f, 0.0f, 0.0f, 1.0f};

  GLuint m_VAO{};            // Vertex Array Object
  GLuint m_VBOVertices{};    // Vertex Buffer Object
  GLuint m_program{};        // Shader Program

  float m_a{1.0f};           // Coeficiente angular (m)
  float m_b{0.0f};           // Coeficiente linear (b)
  std::vector<glm::vec2> m_graphPoints; // Armazena os pontos do gráfico

  // Variáveis para os parâmetros financeiros (Mutáveis)
  float m_P = 1000.0f;       // Investimento inicial
  float m_PMT = 200.0f;      // Contribuição mensal
  float m_i = 0.01f;         // Taxa de juros mensal
  int m_nMax = 120;          // Número de meses (10 anos)

  // Métodos
  void setupModel();
  void calculateGraphPoints(); // Método para calcular os pontos do gráfico

  // Getters e Setters para as variáveis financeiras
  float getInvestimentoInicial() const { return m_P; }
  void setInvestimentoInicial(float value) { m_P = value; }

  float getContribuicaoMensal() const { return m_PMT; }
  void setContribuicaoMensal(float value) { m_PMT = value; }

  float getTaxaJurosMensal() const { return m_i; }
  void setTaxaJurosMensal(float value) { m_i = value; }

  int getNumeroMeses() const { return m_nMax; }
  void setNumeroMeses(int value) { m_nMax = value; }
};

#endif
