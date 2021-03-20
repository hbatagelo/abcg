#include <algorithm>
#include <sstream>

#include "game.hpp"

void Game::initializeGrid(std::array<std::array<int, 100>, 100> origin) {
    grid = origin;
    liveCells.clear();
        for (int x = 0; x < 100; x++) {
            for (int y = 0; y < 100; y++) {
                if (grid.at(y).at(x) == 1) 
                    liveCells.push_back(std::make_tuple(x, y));
            }
        }
}

std::array<std::array<int, 100>, 100> Game::getGrid() {
    return grid;
}

int Game::getCell(int x, int y) {
    return grid.at(y).at(x);
}

void Game::step(int generations) {
    std::array<std::array<int, 100>, 100> nextBoard = grid;
    if (generations > 0) {
        liveCells.clear();
        int qty;
        int currentState;
        for (int x = 0; x < 100; x++) {
            for (int y = 0; y < 100; y++) {
                qty = countLivingNeighbors(x, y);
                currentState = getCell(x, y);
                if (currentState == 1) {
                    // Cell dies
                    if (!(qty == 2 || qty == 3)) {
                        nextBoard.at(y).at(x) = 0;
                    } else {
                        liveCells.push_back(std::make_tuple(x, y));
                    }
                } else {
                    // Cell lives
                    if (qty == 3) {
                        nextBoard.at(y).at(x) = 1;
                        liveCells.push_back(std::make_tuple(x, y));
                    }
                }
            }
        }
        grid = nextBoard;
        currentGeneration++;
        step(generations - 1);
    }
}

int Game::countLivingNeighbors(int x, int y) {
    int x_n;
    int y_n;
    int qty = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0)
                continue;
            
            x_n = x + i;
            y_n = y + j;

            if (x_n < 0) x_n = 99;
            if (x_n > 99) x_n = 0;

            if (y_n < 0) y_n = 99;
            if (y_n > 99) y_n = 0;
            
            qty += getCell(x_n, y_n);
        }
    }

    return qty;
}

std::vector<std::tuple<int, int>> Game::getLiveCells() {
    if (currentGeneration == 0) {
        liveCells.clear();
        for (int x = 0; x < 100; x++) {
            for (int y = 0; y < 100; y++) {
                if (grid.at(y).at(x) == 1) 
                    liveCells.push_back(std::make_tuple(x, y));
            }
        }
    }
    return liveCells;
}

Game::GameState Game::getGameState() {
    return gameState;
}

void Game::toggleState() {
    if (gameState == Game::GameState::PLAYING) {
        gameState = Game::GameState::PAUSED;
    } else {
        gameState = Game::GameState::PLAYING;
    }
}