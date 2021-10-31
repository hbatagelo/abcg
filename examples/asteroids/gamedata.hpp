#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>


// Uma classe do tipo Enum permite que vc converta um valor da classe em um numero conforme a ordem desse valor na classe
// um get_index('c', ['a','b','c','d'])  seria a mesma coisa que converter "Letter::c" de "enum class Letter {a,b,c,d}"
enum class Input { Right, Left, Down, Up, Fire }; // Descreve as ações da nave
enum class State { Playing, GameOver, Win };   // Descreve os possíveis estados do jogo

struct GameData {
  State m_state{State::Playing};
  // m_input vai ser alterado futuramente pela função OpenGLWindow::handleEvent
  std::bitset<5> m_input;  // [fire, up, down, left, right]
  // m_input é 00000, se usamos m_input.set(2) teremos 00100, para voltar resetamos o bit
};

#endif