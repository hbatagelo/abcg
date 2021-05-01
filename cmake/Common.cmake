# Generate compile_commands.json to make it easier to work with clang based
# tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  set(OPTIONS_TARGET options)
  set(SANITIZERS_TARGET sanitizers)
  set(WARNINGS_TARGET warnings)
  add_library(${OPTIONS_TARGET} INTERFACE)
  add_library(${SANITIZERS_TARGET} INTERFACE)
  add_library(${WARNINGS_TARGET} INTERFACE)

  # Standard compiler warnings
  include(${CMAKE_CURRENT_LIST_DIR}/Warnings.cmake)
  set_project_warnings(${WARNINGS_TARGET} PROJECT_WARNINGS)

  # Sanitizer options if supported by compiler
  include(${CMAKE_CURRENT_LIST_DIR}/Sanitizers.cmake)
  enable_sanitizers(${SANITIZERS_TARGET})

  # Static analysis
  include(${CMAKE_CURRENT_LIST_DIR}/StaticAnalyzers.cmake)
endif()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(
    STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE
      RelWithDebInfo
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

# ccache
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

# IPO
option(ENABLE_IPO "Enable Interprocedural Optimization" ON)
if(ENABLE_IPO)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.9)
    cmake_policy(SET CMP0069 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
  endif()

  include(CheckIPOSupported)
  check_ipo_supported(RESULT result OUTPUT output)

  if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message("IPO is not supported")
  endif()
endif()

# Conan
option(ENABLE_CONAN "Use Conan Package Manager" OFF)
if(ENABLE_CONAN AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  # Set up some extra Conan dependencies based on our needs before loading Conan
  set(CONAN_SETTINGS "")
  set(CONAN_EXTRA_REQUIRES "")
  set(CONAN_EXTRA_OPTIONS "")
  set(CONAN_IMPORTS "")

  # fmt
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} fmt/7.1.3)

  # imGUI
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} imgui/1.79)
  set(CONAN_IMPORTS ${CONAN_IMPORTS} "./res/bindings, *.cpp -> ../bindings"
                    "./res/bindings, *.h -> ../bindings")

  # CPPItertools
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} cppitertools/2.0)

  # GLEW
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} glew/2.1.0)

  # GLM
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} glm/0.9.9.8)

  # ms-gsl
  set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} ms-gsl/3.1.0)

  include(${CMAKE_CURRENT_LIST_DIR}/Conan.cmake)
  run_conan()

  # C++20
  target_compile_features(${OPTIONS_TARGET} INTERFACE cxx_std_20)

  # Conan aggregated header's folders
  target_include_directories(${OPTIONS_TARGET} SYSTEM
                             INTERFACE ${CONAN_INCLUDE_DIRS})

  # Conan libs
  target_link_libraries(${OPTIONS_TARGET} INTERFACE ${CONAN_LIBS})
endif()

# ABCg for users
include(${CMAKE_CURRENT_LIST_DIR}/ABCg.cmake)
