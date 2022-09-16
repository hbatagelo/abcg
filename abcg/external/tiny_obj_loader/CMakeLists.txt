cmake_minimum_required(VERSION 3.5)

project(tiny_obj_loader)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} SYSTEM
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
