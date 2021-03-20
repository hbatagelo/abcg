project(CrazyCastle)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp
                               Castle.cpp )

enable_abcg(${PROJECT_NAME})
