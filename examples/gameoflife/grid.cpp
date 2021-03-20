#include <fmt/core.h>

#include <array>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

#include "grid.hpp"

void Grid::initializeGL(GLuint program) {
    terminateGL();

    m_program = program;
    m_colorLoc = glGetUniformLocation(m_program, "color");

    restart();
}

void Grid::paintGL() {
    glUseProgram(m_program);

    for (auto &cell : m_cells) {
        glBindVertexArray(cell.m_vao);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    glUseProgram(0);
}

void Grid::terminateGL() {
    for (auto &cell : m_cells) {
        glDeleteBuffers(1, &cell.m_vboPositions);
        glDeleteBuffers(1, &cell.m_vboColors);
        glDeleteVertexArrays(1, &cell.m_vao);
    }
}

void Grid::update() {
    if (game.getGameState() == Game::GameState::PLAYING) {
        game.step(1);
        m_cells.clear();
        for (auto &cell : game.getLiveCells()) {
            m_cells.push_back(createCell(std::get<0>(cell), std::get<1>(cell), step));
        }
    }
}

Grid::Cell Grid::createCell(int x, int y, float step) {
    Cell cell;

    // Create geometry
    std::vector<glm::vec2> positions{};
    positions.emplace_back(-1 + x * step, 1 - y * step);
    positions.emplace_back(-1 + (x + 1) * step, 1 - y * step);
    positions.emplace_back(-1 + x * step, 1 - (y + 1) * step);
    positions.emplace_back(-1 + (x + 1) * step, 1 - (y + 1) * step);

    // Generate VBO
    glGenBuffers(1, &cell.m_vboPositions);
    glBindBuffer(GL_ARRAY_BUFFER, cell.m_vboPositions);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generate VBO of colors
    glGenBuffers(1, &cell.m_vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, cell.m_vboColors);
    glBufferData(GL_ARRAY_BUFFER, cell.m_color.size() * sizeof(glm::vec3), cell.m_color.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Get location of attributes in the program
    GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
    GLint colorAttribute{glGetAttribLocation(m_program, "inColor")};

    // Create VAO
    glGenVertexArrays(1, &cell.m_vao);

    // Bind vertex attributes to current VAO
    glBindVertexArray(cell.m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, cell.m_vboPositions);
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, cell.m_vboColors);
    glEnableVertexAttribArray(colorAttribute);
    glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // End of binding to current VAO
    glBindVertexArray(0);

    return cell;  
}

void Grid::toggleCellState(float x, float y) {
    if (game.getGameState() == Game::GameState::PAUSED) {
        int x_index = 49 + static_cast<int>(ceil(x / step));
        int y_index = 49 + static_cast<int>(ceil(-y / step));

        auto currentGrid = game.getGrid();
        currentGrid.at(y_index).at(x_index) = currentGrid.at(y_index).at(x_index) == 1 ? 0 : 1;

        game.initializeGrid(currentGrid);

        m_cells.clear();
        for (auto &cell : game.getLiveCells()) {
            m_cells.push_back(createCell(std::get<0>(cell), std::get<1>(cell), step));
        }
    }
}

void Grid::clearGrid() {
    if (game.getGameState() == Game::GameState::PAUSED) {
        std::array<std::array<int, 100>, 100> grid{};
        std::array<int, 100> emptyRow{};
        emptyRow.fill(0);
        grid.fill(emptyRow);

        game.initializeGrid(grid);

        m_cells.clear();
    }
}

void Grid::restart() {
    // Create grid
    m_cells.clear();

    std::array<std::array<int, 100>, 100> grid{};
    std::array<int, 100> emptyRow{};
    emptyRow.fill(0);
    grid.fill(emptyRow);
    
    grid.at(46).at(14) = 1;
    grid.at(47).at(14) = 1;
    
    grid.at(46).at(15) = 1;
    grid.at(47).at(15) = 1;

    grid.at(46).at(24) = 1;
    grid.at(47).at(24) = 1;
    grid.at(48).at(24) = 1;

    grid.at(45).at(25) = 1;
    grid.at(49).at(25) = 1;
    
    grid.at(44).at(26) = 1;
    grid.at(50).at(26) = 1;
    
    grid.at(44).at(27) = 1;
    grid.at(50).at(27) = 1;

    grid.at(47).at(28) = 1;
    
    grid.at(45).at(29) = 1;
    grid.at(49).at(29) = 1;

    grid.at(46).at(30) = 1;
    grid.at(47).at(30) = 1;
    grid.at(48).at(30) = 1;

    grid.at(47).at(31) = 1;

    grid.at(44).at(34) = 1;
    grid.at(45).at(34) = 1;
    grid.at(46).at(34) = 1;

    grid.at(44).at(35) = 1;
    grid.at(45).at(35) = 1;
    grid.at(46).at(35) = 1;

    grid.at(43).at(36) = 1;
    grid.at(47).at(36) = 1;

    grid.at(42).at(38) = 1;
    grid.at(43).at(38) = 1;
    grid.at(47).at(38) = 1;
    grid.at(48).at(38) = 1;

    grid.at(44).at(48) = 1;
    grid.at(45).at(48) = 1;

    grid.at(44).at(49) = 1;
    grid.at(45).at(49) = 1;

    game.initializeGrid(grid);
    for (auto &cell : game.getLiveCells()) {
        m_cells.push_back(createCell(std::get<0>(cell), std::get<1>(cell), step));
    }

    if (game.getGameState() == Game::GameState::PLAYING)
        game.toggleState();
}