#include "openglwindow.hpp"

//#include <conio.h>
#include <fmt/core.h>
#include <imgui.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <random>
#include <sstream>
#include <string>  // Necessário para usar strings

using namespace std;

/*
int gen_number() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 6);

  return dis(gen);
}
*/

void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);

  // Start pseudo-random number generator
  // auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  // m_randomEngine.seed(seed);

  // std::uniform_int_distribution<> dis(0, 9);
  // static double tempoTest = 0;

  TempoExtra = 10;
  score = 0;

  // gera de 0 a 10 para ser o valor inicial de corte
  numCorte = rand() % 10;
  // gera de 0 a 1, 0 = maior e 1 = menor
  maiorMenor = rand() % 2;
  if (maiorMenor == 0) {
    maiorMenorStr = "maiores";
  } else {
    maiorMenorStr = "menores";
  }
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      tabuleiro[i][j] = (rand() % 10);
    }
  }
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
               m_clearColor[3]);
  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);
}
void OpenGLWindow::paintUI() {
  ImGui::SetNextWindowSize(ImVec2(400, 500));
  auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize};
  ImGui::Begin("Jogo Maior Menor", nullptr, flags);
  {
    bool save{};

    // Exemplo de Menubar
    // Menu Bar
    if (ImGui::BeginMenuBar()) {
      // File menu
      if (ImGui::BeginMenu("Game")) {
        ImGui::MenuItem("Restart", nullptr, &save);
        ImGui::EndMenu();
      }
      // View menu
      /*
      if (ImGui::BeginMenu("View")) {
        ImGui::MenuItem("Show Compliment", nullptr, &showCompliment);
        ImGui::EndMenu();
      }
      */
      ImGui::EndMenuBar();
    }

    tempoJogo = TempoExtra - m_restartWaitTimer.elapsed();
    // m_restartWaitTimer.elapsed()

    if (tempoJogo > 0) {
      ImGui::Text("Tempo Restante: %.2f", tempoJogo);

      ImGui::Text("Selecione os numeros %s que: %d", maiorMenorStr.c_str(),
                  numCorte);

      ImGui::Text("Score: %d", score);
      // Criando as colunas
      ImGui::Columns(3, "locations");

      // primeira linha = 1
      // 1x1
      // ImGui::Text("Oiê");

      // Construindo o tabuleiro
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          // tempoJogo = 10 - m_restartWaitTimer.elapsed();
          // auto offset{i * 2 + j};
          // ImGui::Text("%d", offset);

          // ImGui::Text("%d", i);
          // ImGui::Text("%d", j);
          // ImGui::Text("%d", tabuleiro[i][j]);
          // tabuleiro[i][j] = i;
          // Create an output string stream
          std::ostringstream streamObj;
          // Add double to stream
          streamObj << tabuleiro[i][j];
          // Get string from output string stream
          std::string strObj = streamObj.str();
          // std::array<char> temp = strObj;
          // std::string tabStr{fmt::format("%d", offset)};

          ImGui::Button(strObj.c_str(), ImVec2(100, 100));

          if (ImGui::IsItemClicked()) {
            if (maiorMenor == 0) {
              // maiores
              if (tabuleiro[i][j] > numCorte) {
                TempoExtra += 1;
                score++;
                tabuleiro[i][j] = rand() % 10;
              } else {
                TempoExtra -= 1;
              }
            } else {
              // menores
              if (tabuleiro[i][j] < numCorte) {
                TempoExtra += 1;
                tabuleiro[i][j] = rand() % 10;
                score++;
              } else {
                TempoExtra -= 1;
              }
            }
          }
          ImGui::NextColumn();  // método para passar para a próxima coluna
        }
        ImGui::Separator();  // método para colocar o separador
      }

      ImGui::Columns(1, "locations");

      // ImGui::NextColumn();  // método para passar para a próxima coluna
      ImGui::Button("sortear novamente!", ImVec2(200, 50));
      if (ImGui::IsItemClicked()) {
        TempoExtra -= 1;
        Sortear();
      }
    } else {
      ImGui::Text("Seu tempo acabou :c");
      ImGui::Text("Fim de Jogo!");
      ImGui::Text("Score: %d", score);
      ImGui::Button("Reiniciar", ImVec2(200, 50));
      if (ImGui::IsItemClicked())
      {
        Restart();
      }
      
    }
  }
  ImGui::End();
}

void OpenGLWindow::Sortear() {
  // gera de 0 a 10 para ser o valor inicial de corte
  numCorte = rand() % 10;
  // gera de 0 a 1, 0 = maior e 1 = menor
  maiorMenor = rand() % 2;
  if (maiorMenor == 0) {
    maiorMenorStr = "maiores";
  } else {
    maiorMenorStr = "menores";
  }
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      tabuleiro[i][j] = (rand() % 10);
    }
  }
}

void OpenGLWindow::Restart() {
  TempoExtra = 10;
  m_restartWaitTimer.restart();
  score = 0;
  Sortear();
}
