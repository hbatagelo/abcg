project(corridaMaluca)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp crazycastle.cpp
                               bullets.cpp castle.cpp starlayers.cpp)

enable_abcg(${PROJECT_NAME})
