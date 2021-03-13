project(dinorun)
add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp 
                               bullets.cpp ship.cpp starlayers.cpp)
enable_abcg(${PROJECT_NAME})
