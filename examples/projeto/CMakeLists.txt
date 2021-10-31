cmake_minimum_required(VERSION 3.11)

project(projeto)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp pads.cpp)

enable_abcg(${PROJECT_NAME})
