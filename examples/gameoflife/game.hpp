#ifndef GAME_HPP_
#define GAME_HPP_

#include <array>
#include <vector>
#include <tuple>
#include <iostream>

#include "abcg.hpp"

class Game {
    public:
        enum class GameState {
            PLAYING,
            PAUSED
        };
        void initializeGrid(std::array<std::array<int, 100>, 100> origin);
        std::array<std::array<int, 100>, 100> getGrid();
        void step(int generations);
        std::vector<std::tuple<int, int>> getLiveCells();
        int getCell(int x, int y);
        Game::GameState getGameState();
        void toggleState(); 

    private:
        // enum class InitialState {
        //     RANDOM,
        //     SMILEY_FACE
        // }
        // Game::InitialState initialState;

        Game::GameState gameState{Game::GameState::PAUSED};

        std::array<std::array<int, 100>, 100> grid{};
        std::vector<std::tuple<int, int>> liveCells{};
        int currentGeneration{0};

        int countLivingNeighbors(int x, int y);
};

#endif