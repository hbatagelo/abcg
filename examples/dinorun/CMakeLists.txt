project(dinorun)
add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp asteroids.cpp
                               dino.cpp starlayers.cpp)
enable_abcg(${PROJECT_NAME})
