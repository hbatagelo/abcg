#ifndef GRID_HPP_
#define GRID_HPP_

#include <vector>

#include "abcg.hpp"
#include "game.hpp"

class OpenGLWindow;

class Grid {
    public:
        void initializeGL(GLuint program);
        void paintGL();
        void terminateGL();
        void update();
        void toggleCellState(float x, float y);

    private:
        friend OpenGLWindow;

        GLuint m_program{};
        GLint m_colorLoc{};

        float step{0.02};

        struct Cell {
            GLuint m_vao{};
            GLuint m_vboPositions{};
            GLuint m_vboColors{};
            std::array<glm::vec3, 4> m_color
            {
                {
                    {1.0f, 0.0f, 0.0f},
                    {1.0f, 0.0f, 0.0f},
                    {1.0f, 0.0f, 0.0f},
                    {1.0f, 0.0f, 0.0f}
                }
            };
        };

        Grid::Cell createCell(int x, int y, float step);

        Game game;

        std::vector<Cell> m_cells{};

        void restart();
        void clearGrid();
};

#endif