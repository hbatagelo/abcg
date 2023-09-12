cmake_minimum_required(VERSION 3.21)

project(imgui)

set(IMGUI_FILES imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp
                imgui_widgets.cpp imgui_impl_sdl2.cpp)

if(${GRAPHICS_API} MATCHES "OpenGL")
  set(IMGUI_FILES ${IMGUI_FILES} imgui_impl_opengl3.cpp)
endif()

if(${GRAPHICS_API} MATCHES "Vulkan")
  set(IMGUI_FILES ${IMGUI_FILES} imgui_impl_vulkan.cpp)
endif()

add_library(${PROJECT_NAME} ${IMGUI_FILES})

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE /usr/local/include /opt/local/include
  PRIVATE ${SDL2_INCLUDE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
else()
  find_package(SDL2 REQUIRED)
  target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIR})
  set(LINK_LIBRARIES ${SDL2_LIBRARY} ${CMAKE_DL_LIBS})
  if(${GRAPHICS_API} MATCHES "Vulkan")
    find_package(Vulkan REQUIRED)
    set(LINK_LIBRARIES ${LINK_LIBRARIES} Vulkan::Headers)
    # Assume entrypoints loaded by volk
    add_compile_definitions(IMGUI_IMPL_VULKAN_NO_PROTOTYPES)
  endif()
  target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBRARIES})
endif()

if(APPLE)
  if(${GRAPHICS_API} MATCHES "OpenGL")
    add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLEW)
  endif()
  target_include_directories(${PROJECT_NAME} PRIVATE /usr/local/include
                                                     /opt/local/include)
endif()
