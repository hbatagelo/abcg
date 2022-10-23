cmake_minimum_required(VERSION 3.11)

project(paint)
add_executable(${PROJECT_NAME} main.cpp 
                            window.cpp 
                            shader.hpp 
                            shape.hpp 
                            rectangle.cpp 
                            circle.cpp
                            triangle.cpp
                            pentagon.cpp
                            star.cpp
)
enable_abcg(${PROJECT_NAME})
