#include <fmt/core.h>
#include <string>
#include "openglwindow.hpp"
#include <imgui.h>

enum StatusCampo { Vazio = 0, O, X };
enum Jogador { Um, Dois };
enum Resultado { Continua = 0, Empate, UmVenceu, DoisVenceu };

Resultado conferirJogo(StatusCampo (&tabuleiro)[9]) {
  // Verifica colunas
  if(tabuleiro[1] == tabuleiro[2] && tabuleiro[1] == tabuleiro[3]){
      return (*tabuleiro == StatusCampo::O)? Resultado::UmVenceu : Resultado::DoisVenceu;
  }
  return Resultado::Continua;
}

void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  abcg::glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
                     m_clearColor[3]);
  // Clear the color buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::OpenGLWindow::paintUI();

  // Our own ImGui widgets go below
  {
    // Window begin
    ImGui::Begin("Jogo da Velha!");

    // Variavel global com o tabuleiro
    static StatusCampo tabuleiro[9] = {StatusCampo::Vazio};
    // Static text
    auto windowSettings{getWindowSettings()};
    const std::array<std::string, 2> msgs_usuario = {"Jogada do O", "Jogada do X"};
    const std::array<std::string, 3> texto_campos = {" ", "O", "X"};
    const std::array<std::string, 4> status_jogo = {"Continua", "Empate", "Jogador O Venceu!", "Jogador X Venceu"};
    
    static Jogador jogador = Jogador::Um;
    
    ImGui::Text(msgs_usuario[jogador].c_str(),
                windowSettings.width, windowSettings.height);

    for(int i = 0; i<3; i++){
      ImGui::Button(texto_campos[tabuleiro[3*i]].c_str(), ImVec2(100, 50));
      if (ImGui::IsItemClicked()) {
        if (tabuleiro[3*i] == StatusCampo::Vazio) {
          Resultado res = conferirJogo(tabuleiro);
          fmt::print("Status jogo: {}", res);
          tabuleiro[3*i] = (jogador == Jogador::Um)? StatusCampo::O : StatusCampo::X;
          jogador = (jogador == Jogador::Um)? Jogador::Dois : Jogador::Um;
        }
        fmt::print("Button pressed.\n");
      }
      for(int j = 1; j<3; j++) {
        ImGui::SameLine();
        ImGui::Button(texto_campos[tabuleiro[3*i+j]].c_str(), ImVec2(100, 50));
        if (ImGui::IsItemClicked()) {
          fmt::print("Button pressed.\n");
          if (tabuleiro[3*i+j] == StatusCampo::Vazio) {
            fmt::print("Button pressed.\n");
            tabuleiro[3*i+j] = (jogador == Jogador::Um)? StatusCampo::O : StatusCampo::X;
            jogador = (jogador == Jogador::Um)? Jogador::Dois : Jogador::Um;
          }
        }
      }
    }
      
    // Window end
    ImGui::End();
  }
}