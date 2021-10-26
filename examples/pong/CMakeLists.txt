project(pong)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp
                               bar.cpp )

enable_abcg(${PROJECT_NAME})
