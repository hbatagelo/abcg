cmake_minimum_required(VERSION 3.5)

project(imgui)

add_library(
  ${PROJECT_NAME} imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_widgets.cpp
                  imgui_impl_sdl.cpp imgui_impl_opengl3.cpp)

# Check if SDL.h is inside a SDL2 subdirectory
find_path(
  SDL_PATH
  NAMES "SDL.h"
  PATH_SUFFIXES "SDL2")
if(NOT SDL_PATH MATCHES "NOTFOUND")
  target_include_directories(${PROJECT_NAME} PUBLIC ${SDL_PATH})
endif()

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE /usr/local/include /opt/local/include
  PRIVATE ${SDL2_INCLUDE_DIRS}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
endif()

if(APPLE)
  add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLEW)
  target_include_directories(
    ${PROJECT_NAME} PRIVATE /usr/local/include /opt/local/include)
endif()