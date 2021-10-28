project(pong)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp
                               barLeft.cpp ball.cpp scenary.cpp)

enable_abcg(${PROJECT_NAME})
