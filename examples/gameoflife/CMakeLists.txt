project(gameoflife)
add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp grid.cpp game.cpp)
enable_abcg(${PROJECT_NAME})