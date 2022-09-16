cmake_minimum_required(VERSION 3.21)

project(external)

add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory(tiny_obj_loader)

target_link_libraries(${PROJECT_NAME} INTERFACE tiny_obj_loader)

if(NOT ENABLE_CONAN OR ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")

  target_include_directories(${PROJECT_NAME} SYSTEM
                             INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

  if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    find_package(SDL2 REQUIRED)
    if(${GRAPHICS_API} MATCHES "OpenGL")
      find_package(OpenGL REQUIRED)
      if(MSVC)
        set(GLEW_USE_STATIC_LIBS
            ON
            CACHE BOOL "Use GLEW static libraries")
      endif()
      find_package(GLEW REQUIRED)
      target_link_libraries(${PROJECT_NAME} INTERFACE OpenGL::GL GLEW::GLEW
                                                      ${SDL2_LIBRARY})
    endif()
    if(${GRAPHICS_API} MATCHES "Vulkan")
      add_subdirectory(glslang)
      add_subdirectory(volk)
      target_link_libraries(${PROJECT_NAME} INTERFACE ${SDL2_LIBRARY} glslang
                                                      SPIRV volk)
    endif()
  endif()

  add_subdirectory(imgui)
  add_subdirectory(cppitertools)
  add_subdirectory(fmt)
  add_subdirectory(glm)
  add_subdirectory(gsl)

  target_include_directories(${PROJECT_NAME} SYSTEM
                             INTERFACE ${SDL2_INCLUDE_DIR})

  if(${GRAPHICS_API} MATCHES "OpenGL")
    target_include_directories(
      ${PROJECT_NAME} SYSTEM
      INTERFACE ${GLEW_INCLUDE_DIRS}
      INTERFACE ${OPENGL_INCLUDE_DIR})
  endif()

  target_link_libraries(${PROJECT_NAME} INTERFACE cppitertools fmt glm gsl
                                                  imgui)
endif()
