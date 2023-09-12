cmake_minimum_required(VERSION 3.21)

project(abcg)

# Where the find_package files are located
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(ABCG_FILES abcgApplication.cpp abcgTimer.cpp abcgException.cpp
               abcgImage.cpp abcgTrackball.cpp abcgWindow.cpp abcgUtil.cpp)

if(${GRAPHICS_API} MATCHES "OpenGL")
  set(ABCG_FILES ${ABCG_FILES} abcgOpenGLError.cpp abcgOpenGLFunction.cpp
                 abcgOpenGLImage.cpp abcgOpenGLShader.cpp abcgOpenGLWindow.cpp)
elseif(${GRAPHICS_API} MATCHES "Vulkan")
  set(ABCG_FILES
      ${ABCG_FILES}
      abcgVulkanBuffer.cpp
      abcgVulkanDevice.cpp
      abcgVulkanError.cpp
      abcgVulkanImage.cpp
      abcgVulkanInstance.cpp
      abcgVulkanPipeline.cpp
      abcgVulkanPhysicalDevice.cpp
      abcgVulkanShader.cpp
      abcgVulkanSwapchain.cpp
      abcgVulkanWindow.cpp)
endif()

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
  if(ENABLE_CONAN)
    set(IMGUI_BINDINGS_DIR "${CMAKE_SOURCE_DIR}/bindings")

    set(ABCG_FILES_WITH_IMGUI ${ABCG_FILES}
                              ${IMGUI_BINDINGS_DIR}/imgui_impl_sdl2.cpp)

    if(${GRAPHICS_API} MATCHES "OpenGL")
      set(ABCG_FILES_WITH_IMGUI ${ABCG_FILES_WITH_IMGUI}
                                ${IMGUI_BINDINGS_DIR}/imgui_impl_opengl3.cpp)
    elseif(${GRAPHICS_API} MATCHES "Vulkan")
      set(ABCG_FILES_WITH_IMGUI ${ABCG_FILES_WITH_IMGUI}
                                ${IMGUI_BINDINGS_DIR}/imgui_impl_vulkan.cpp)
    endif()

    add_library(${PROJECT_NAME} ${ABCG_FILES_WITH_IMGUI})

    target_include_directories(${PROJECT_NAME}
                               PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${PROJECT_NAME} SYSTEM
                               PUBLIC ${IMGUI_BINDINGS_DIR})
    target_link_libraries(
      ${PROJECT_NAME}
      PUBLIC external
      PUBLIC ${OPTIONS_TARGET})

    # Enable warnings only for selected files
    set_source_files_properties(${ABCG_FILES} PROPERTIES COMPILE_OPTIONS
                                                         "${PROJECT_WARNINGS}")
  else()
    find_package(SDL2_image REQUIRED)

    add_library(${PROJECT_NAME} ${ABCG_FILES})

    target_include_directories(
      ${PROJECT_NAME}
      PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
      PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})
    target_link_libraries(
      ${PROJECT_NAME}
      PUBLIC external
      PUBLIC ${SDL2_IMAGE_LIBRARIES})
  endif()

  # Use sanitizers in debug mode
  if(CMAKE_BUILD_TYPE MATCHES "DEBUG|Debug")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SANITIZERS_TARGET})
  endif()

  target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

  if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
  endif()
endif()

# Convert binary assets to header
set(NEW_HEADER_FILE "abcgEmbeddedFonts.hpp")

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
