cmake_minimum_required(VERSION 3.11)

project(firstapp)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp)

enable_abcg(${PROJECT_NAME})

