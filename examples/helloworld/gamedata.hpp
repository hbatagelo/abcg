#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { LDown, LUp, RDown, RUp};
enum class State { Playing, LeftWin, RightWin };

struct GameData {
  State m_state{State::Playing};
  std::bitset<4> m_input;  // [LDown, LUp, RDown, RUp]
};

#endif