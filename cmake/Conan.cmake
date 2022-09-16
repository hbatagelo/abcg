# From: https://github.com/conan-io/cmake-conan

# Download Conan packages and extras specified in CONAN_EXTRA_REQUIRES
macro(run_conan)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
  list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

  # Download automatically, you can also just copy the conan.cmake file
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(
      STATUS
        "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(
      DOWNLOAD
      "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
      "${CMAKE_BINARY_DIR}/conan.cmake"
      TLS_VERIFY ON)
  endif()

  include(${CMAKE_BINARY_DIR}/conan.cmake)

  # Some remotes (e.g. bincrafters) requires conan clients to have revisions
  # enabled
  set(ENV{CONAN_REVISIONS_ENABLED} 1)

  conan_check()

  # If CMAKE_BUILD_TYPE is not defined (e.g. building with Visual Studio
  # generator), set build type to Release
  if(NOT DEFINED CMAKE_BUILD_TYPE)
    # Notice that this won't work when building from VS Code using a build
    # variant other than Release! This can be fixed by adding the following
    # "CMake Tools" settings to settings.json:
    #
    # "cmake.configureSettings": { "CMAKE_BUILD_TYPE": "${buildType}" }
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release")
  endif()

  # Launch conan install
  conan_cmake_configure(
    REQUIRES
    ${CONAN_EXTRA_REQUIRES}
    OPTIONS
    ${CONAN_EXTRA_OPTIONS}
    IMPORTS
    ${CONAN_IMPORTS}
    GENERATORS
    cmake_find_package)

  conan_cmake_autodetect(settings)

  conan_cmake_install(
    PATH_OR_REFERENCE
    .
    BUILD
    missing
    PROFILE
    default
    SETTINGS
    ${settings}
    ${PROJECT_NAME}:build_type=${CMAKE_BUILD_TYPE}
    # Third-party libraries are always built in Release mode
    build_type=Release)

endmacro()
