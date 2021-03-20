project(Crazycastle)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp
                               castle.cpp )

enable_abcg(${PROJECT_NAME})
