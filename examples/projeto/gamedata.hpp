#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>


// Uma classe do tipo Enum permite que vc converta um valor da classe em um numero conforme a ordem desse valor na classe
// um get_index('c', ['a','b','c','d'])  seria a mesma coisa que converter "Letter::c" de "enum class Letter {a,b,c,d}"
enum class Input { LDown, LUp, RDown, RUp }; // Descreve as ações das raquetes
enum class State { Playing, WinLeft, WinRight };   // Descreve os possíveis estados do jogo

struct GameData {
  State m_state{State::Playing};
  // m_input vai ser alterado futuramente pela função OpenGLWindow::handleEvent
  std::bitset<4> m_input;  // [LDown, LUp, RDown, RUp]
  // m_input é 00, se usamos m_input.set(1) teremos 01, para voltar resetamos o bit usamos reset(1)
};

#endif