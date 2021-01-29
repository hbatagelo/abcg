cmake_minimum_required(VERSION 3.11)

project(external)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} SYSTEM
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT ENABLE_CONAN OR ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")

  if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(SDL2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} INTERFACE OpenGL::GL GLEW::GLEW
                                                    ${SDL2_LIBRARIES})
  endif()

  add_subdirectory(imgui)
  add_subdirectory(cppitertools)
  add_subdirectory(fmt)
  add_subdirectory(glm)
  add_subdirectory(gsl)

  target_include_directories(
    ${PROJECT_NAME} SYSTEM
    INTERFACE ${GLEW_INCLUDE_DIRS}
    INTERFACE ${OPENGL_INCLUDE_DIR}
    INTERFACE ${SDL2_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} INTERFACE cppitertools fmt glm gsl
                                                  imgui)
endif()
