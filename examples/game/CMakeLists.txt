project(game)
add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp ghost.cpp pacmans.cpp)
enable_abcg(${PROJECT_NAME})
