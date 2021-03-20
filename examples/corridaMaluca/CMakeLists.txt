project(corridaMaluca)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp CrazyCastle.cpp
                               bullets.cpp Castle.cpp starlayers.cpp)

enable_abcg(${PROJECT_NAME})
