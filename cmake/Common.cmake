# Generate compile_commands.json to make it easier to work with clang based
# tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Graphics API
set(GRAPHICS_API
    OpenGL
    CACHE STRING "Choose the graphics API.")
set_property(CACHE GRAPHICS_API PROPERTY STRINGS "OpenGL" "Vulkan" "None")

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  # Conan
  option(ENABLE_CONAN "Use Conan Package Manager" OFF)

  # mold
  if(NOT MSVC)
    option(ENABLE_MOLD "Enable mold (Modern Linker)" ON)
  endif()

  # IPO
  if(NOT ENABLE_MOLD)
    option(ENABLE_IPO "Enable Interprocedural Optimization" ON)
  endif()

  set(OPTIONS_TARGET options)
  set(SANITIZERS_TARGET sanitizers)
  set(WARNINGS_TARGET warnings)
  add_library(${SANITIZERS_TARGET} INTERFACE)
  add_library(${WARNINGS_TARGET} INTERFACE)
  add_library(${OPTIONS_TARGET} INTERFACE)

  # Standard compiler warnings
  include(${CMAKE_CURRENT_LIST_DIR}/Warnings.cmake)
  set_project_warnings(${WARNINGS_TARGET} PROJECT_WARNINGS)

  # Sanitizer options if supported by compiler
  include(${CMAKE_CURRENT_LIST_DIR}/Sanitizers.cmake)
  enable_sanitizers(${SANITIZERS_TARGET})
endif()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

# Conan
if(ENABLE_CONAN)
  # Set up some extra Conan dependencies based on our needs before loading Conan
  set(CONAN_SETTINGS "")
  set(CONAN_EXTRA_REQUIRES "")
  set(CONAN_EXTRA_OPTIONS "")
  set(CONAN_IMPORTS "")

  # fmt
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} fmt/10.0.0)

  # imGUI
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} imgui/1.89.4) # 1.87
  set(CONAN_IMPORTS
      ${CONAN_IMPORTS} "./res/bindings, *.cpp -> ${CMAKE_SOURCE_DIR}/bindings"
      "./res/bindings, *.h -> ${CMAKE_SOURCE_DIR}/bindings")

  # CPPItertools
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} cppitertools/2.1)

  # GLM
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} glm/0.9.9.8)

  # ms-gsl
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} ms-gsl/4.0.0)

  # SDL2
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} sdl/2.26.5)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl*:pulse=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl*:nas=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl*:wayland=False)

  # SDL2_image
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} sdl_image/2.0.5)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:bmp=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:gif=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:lbm=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:pcx=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:pnm=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:svg=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:tga=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:xcf=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:xpm=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:xv=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:with_libtiff=False)
  set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sdl_image*:with_libwebp=False)

  if(${GRAPHICS_API} MATCHES "OpenGL")
    # GLEW
    set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} glew/2.2.0)
  elseif(${GRAPHICS_API} MATCHES "Vulkan")
    set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} glslang/11.7.0)
    set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} volk/1.3.243.0)
  endif()

  include(${CMAKE_CURRENT_LIST_DIR}/Conan.cmake)
  run_conan()

  find_package(fmt)
  find_package(imgui)
  find_package(cppitertools)
  find_package(glm)
  find_package(Microsoft.GSL)
  find_package(SDL2)
  find_package(SDL2_image)

  if(${GRAPHICS_API} MATCHES "OpenGL")
    find_package(GLEW)
  elseif(${GRAPHICS_API} MATCHES "Vulkan")
    find_package(glslang)
    find_package(volk)
    # Assume entrypoints loaded by volk
    add_compile_definitions(IMGUI_IMPL_VULKAN_NO_PROTOTYPES)
  endif()

  # C++20
  target_compile_features(${OPTIONS_TARGET} INTERFACE cxx_std_20)

  # Conan libs
  set(LINK_LIBRARIES
      fmt::fmt
      imgui::imgui
      cppitertools::cppitertools
      glm::glm
      Microsoft.GSL::GSL
      SDL2::SDL2
      SDL2_image::SDL2_image)

  if(${GRAPHICS_API} MATCHES "OpenGL")
    set(LINK_LIBRARIES ${LINK_LIBRARIES} GLEW::GLEW)
  elseif(${GRAPHICS_API} MATCHES "Vulkan")
    set(LINK_LIBRARIES ${LINK_LIBRARIES} glslang::glslang volk::volk)
  endif()

  target_link_libraries(${OPTIONS_TARGET} INTERFACE ${LINK_LIBRARIES})

endif()

# mold
if(ENABLE_MOLD)
  find_program(MOLD mold)
  if(MOLD)
    message("Using mold")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
  else()
    message("Not using mold - not found")
  endif()
endif()

# IPO
if(ENABLE_IPO)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.9)
    cmake_policy(SET CMP0069 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
  endif()

  include(CheckIPOSupported)
  check_ipo_supported(RESULT result OUTPUT output)

  if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    if(CMAKE_COMPILER_IS_GNUCXX)
      list(APPEND CMAKE_C_COMPILE_OPTIONS_IPO "-flto=auto")
      list(APPEND CMAKE_CXX_COMPILE_OPTIONS_IPO "-flto=auto")
    endif()
  else()
    message("IPO is not supported: ${output}")
  endif()
endif()

# ccache
if(NOT MSVC)
  if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten" OR NOT ${CMAKE_GENERATOR}
                                                      MATCHES "Ninja")
    find_program(CCACHE ccache)
    if(CCACHE)
      message("Using ccache")
      set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
    else()
      message("Not using ccache - not found")
    endif()
  else()
    message("Not using ccache - disabled for Ninja builds with Emscripten")
  endif()
else()
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
  if(ENABLE_CONAN AND ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    # Silence warnings about linking the release DLL version of the CRT
    # Third-party libraries are always compiled in release mode
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
  endif()
endif()

# ABCg for users
include(${CMAKE_CURRENT_LIST_DIR}/ABCg.cmake)
