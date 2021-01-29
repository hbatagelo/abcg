cmake_minimum_required(VERSION 3.5)

project(fmt)

add_library(${PROJECT_NAME} format.cc)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
