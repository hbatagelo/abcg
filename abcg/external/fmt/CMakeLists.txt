cmake_minimum_required(VERSION 3.16)

project(fmt)

add_library(${PROJECT_NAME} format.cc)

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  target_compile_options(${PROJECT_NAME} PUBLIC "-w")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
