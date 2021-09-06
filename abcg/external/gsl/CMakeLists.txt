cmake_minimum_required(VERSION 3.5)

project(gsl)

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/gsl")
