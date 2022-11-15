cmake_minimum_required(VERSION 3.11)

project(boids)
add_executable(${PROJECT_NAME} main.cpp window.cpp boid.cpp camera.cpp space.cpp)
enable_abcg(${PROJECT_NAME})
