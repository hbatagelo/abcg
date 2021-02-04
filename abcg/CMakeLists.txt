cmake_minimum_required(VERSION 3.11)

project(abcg)

# Where the find_package files are located
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(ABCG_FILES
    abcg_application.cpp
    abcg_elapsedtimer.cpp
    abcg_exception.cpp
    abcg_image.cpp
    abcg_openglfunctions.cpp
    abcg_openglwindow.cpp
    abcg_string.cpp
    abcg_trackball.cpp)

add_subdirectory(external)

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")

  add_library(${PROJECT_NAME} ${ABCG_FILES})

  target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${PROJECT_NAME} PUBLIC external)
  target_compile_options(
    ${PROJECT_NAME}
    PUBLIC "-std=c++20"
    PUBLIC "-Oz"
    PUBLIC "-sUSE_SDL=2"
    PUBLIC "-sUSE_SDL_IMAGE=2")

  if(${CMAKE_GENERATOR} MATCHES "Ninja")
    set_target_properties(
      ${PROJECT_NAME} PROPERTIES COMPILE_FLAGS
                                 -sSDL2_IMAGE_FORMATS="[""jpg"",""png""]")
  else()
    set_target_properties(
      ${PROJECT_NAME} PROPERTIES COMPILE_FLAGS
                                 -sSDL2_IMAGE_FORMATS='["jpg","png"]')
  endif()

else()

  find_package(SDL2 REQUIRED)
  find_package(SDL2_image REQUIRED)

  if(ENABLE_CONAN)
    add_library(${PROJECT_NAME} ${ABCG_FILES} ../bindings/imgui_impl_sdl.cpp
                                ../bindings/imgui_impl_opengl3.cpp)
    target_include_directories(
      ${PROJECT_NAME}
      PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
      PUBLIC ${SDL2_IMAGE_DIRS}
	  PUBLIC ${SDL2_INCLUDE_DIR}
      PUBLIC "../bindings")
    target_link_libraries(
      ${PROJECT_NAME}
      PUBLIC external
      PUBLIC ${OPTIONS_TARGET}
	  PUBLIC ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} GL dl)

    # Enable warnings only for selected files
    set_source_files_properties(${ABCG_FILES} PROPERTIES COMPILE_OPTIONS
                                                         "${PROJECT_WARNINGS}")
  else()

    add_library(${PROJECT_NAME} ${ABCG_FILES})

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
      PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
      PUBLIC ${SDL2_IMAGE_DIRS}
	  PUBLIC ${SDL2_INCLUDE_DIR})
    target_link_libraries(
      ${PROJECT_NAME}
      PUBLIC external
	  PUBLIC ${SDL2_LIBRARY}
      PUBLIC ${SDL2_IMAGE_LIBRARIES})
  endif()

  # Use sanitizers in debug mode
  if(CMAKE_BUILD_TYPE MATCHES "DEBUG|Debug")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SANITIZERS_TARGET})
  endif()

  target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

endif()

# Convert binary assets to header
set(NEW_HEADER_FILE "abcg_embeddedfonts.hpp")

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${NEW_HEADER_FILE}")
  include(../cmake/bin2h.cmake)
  set(SOURCE_FILES "assets/Inconsolata-Medium.ttf")

  message("Embedding following files into header file ${NEW_HEADER_FILE}:")

  file(WRITE ${NEW_HEADER_FILE} "#include <array>\n\n")

  foreach(file ${SOURCE_FILES})
    message("   ${file}")
    get_filename_component(variableName ${file} NAME)
    bin2h(SOURCE_FILE ${file} HEADER_FILE ${NEW_HEADER_FILE} VARIABLE_NAME
          ${variableName})
    file(APPEND ${NEW_HEADER_FILE} "\n")
  endforeach()
endif()
