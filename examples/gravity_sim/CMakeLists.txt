cmake_minimum_required(VERSION 3.11)

project(gravity_sim)
add_executable(${PROJECT_NAME} main.cpp window.cpp particle.cpp)
enable_abcg(${PROJECT_NAME})
