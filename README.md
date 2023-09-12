# ABCg

![build workflow](https://github.com/hbatagelo/abcg/actions/workflows/build.yml/badge.svg)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/hbatagelo/abcg)](https://github.com/hbatagelo/abcg/releases/latest)

Development framework accompanying the course [MCTA008-17 Computer Graphics](http://professor.ufabc.edu.br/~harlen.batagelo/cg/) at [UFABC](https://www.ufabc.edu.br/).

[Documentation](https://hbatagelo.github.io/abcg/abcg/doc/html/) | [Release notes](CHANGELOG.md)

ABCg is a lightweight C++ framework that simplifies the development of 3D graphics applications based on [OpenGL](https://www.opengl.org), [OpenGL ES](https://www.khronos.org), [WebGL](https://www.khronos.org/webgl/), and [Vulkan](https://www.vulkan.org). It is designed for the tutorials and assignments of the course "MCTA008-17 Computer Graphics" taught at Federal University of ABC (UFABC).

***

## Main features

*   Supported platforms: Linux, macOS, Windows, WebAssembly.
*   Supported backends: OpenGL 3.3+, OpenGL ES 3.0+, WebGL 2.0 (via Emscripten), Vulkan 1.3.
*   Applications that use the common subset of functions between OpenGL 3.3 and OpenGL ES 3.0 can be built for WebGL 2.0 using the same source code.
*   OpenGL functions can be qualified with the `abcg::` namespace to enable throwing exceptions with descriptive GL error messages that include the source code location.
*   Includes helper classes and functions for loading textures (using [SDL\_image](https://www.libsdl.org/projects/SDL_image/)), loading OBJ 3D models (using [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)), and compiling GLSL shaders to SPIR-V with [glslang](https://github.com/KhronosGroup/glslang).

***

## Requirements

The following minimum requirements are shared among all platforms:

*   [CMake](https://cmake.org/) 3.21.
*   A C++ compiler with at least partial support for C++20 (tested with GCC 12, Clang 16, MSVC 17, and emcc 3.1.42).
*   A system with support for OpenGL 3.3 (OpenGL backend) or Vulkan 1.3 (Vulkan backend). Conformant software rasterizers such as Mesa's [Gallium llvmpipe](https://docs.mesa3d.org/drivers/llvmpipe.html) and lavapipe (post Jun 2022) are supported. Mesa's [D3D12](https://devblogs.microsoft.com/directx/directx-heart-linux/) backend on [WSL 2.0](https://docs.microsoft.com/en-us/windows/wsl/install) is supported as well.

For WebAssembly:

*   [Emscripten](https://emscripten.org/).
*   A browser with support for WebGL 2.0.

For building desktop applications:

*   [SDL](https://www.libsdl.org/) 2.0.
*   [SDL\_image](https://www.libsdl.org/projects/SDL_image/) 2.0.
*   [GLEW](http://glew.sourceforge.net/) 2.2.0 (required for OpenGL-based applications).
*   [Vulkan](https://www.lunarg.com/vulkan-sdk/) 1.3 (required for Vulkan-based applications).

Desktop dependencies can be resolved automatically with [Conan](https://conan.io/), but it is disabled by default. To use Conan, install Conan 1.47 or a later 1.\* version (ABCg is not compatible with Conan 2.0!) and then configure CMake with `-DENABLE_CONAN=ON`.

The default renderer backend is OpenGL (CMake option `GRAPHICS_API=OpenGL`). To use the Vulkan backend, configure CMake with `-DGRAPHICS_API=Vulkan`.

***

## Installation and usage

Start by cloning the repository:

    # Get abcg repo
    git clone https://github.com/hbatagelo/abcg.git

    # Enter the directory
    cd abcg

Follow the instructions below to build the "Hello, World!" sample located in `abcg/examples/helloworld`.

### Windows

*   Run `build-vs.bat` for building with the Visual Studio 2022 toolchain.
*   Run `build.bat` for building with GCC (MinGW-w64).

`build-vs.bat` and `build.bat` accept two optional arguments: (1) the build type, which is `Release` by default, and (2) an extra CMake option. For example, for a `Debug` build with `-DENABLE_CONAN=ON` using VS 2022, run

    build-vs.bat Debug -DENABLE_CONAN=ON

### Linux and macOS

Run `./build.sh`.

The script accepts two optional arguments: (1) the build type, which is `Release` by default, and (2) an extra CMake option. For example, for a `Debug` build with `-DENABLE_CONAN=ON`, run

    ./build.sh Debug -DENABLE_CONAN=ON

### WebAssembly

1.  Run `build-wasm.bat` (Windows) or `./build-wasm.sh` (Linux/macOS).
2.  Run `runweb.bat` (Windows) or `./runweb.sh` (Linux/macOS) for setting up a local web server.
3.  Open <http://localhost:8080/helloworld.html>.

***

## Docker setup

ABCg can be built in a [Docker](https://www.docker.com/) container. The Dockerfile provided is based on Ubuntu 22.04 and includes Emscripten.

1.  Create the Docker image (`abcg`):

        sudo docker build -t abcg .

2.  Create the container (`abcg_container`):

        sudo docker create -it \
          -p 8080:8080 \
          -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
          -e DISPLAY \
          --name abcg_container abcg

3.  Start the container:

        sudo docker start -ai abcg_container

    On NVIDIA GPUs, install the [NVIDIA Container Toolkit](https://github.com/NVIDIA/nvidia-docker) to allow the container to use the host's NVIDIA driver and X server. Expose the X server with `sudo xhost +local:root` before starting the container.

***

## License

ABCg is licensed under the MIT License. See [LICENSE](https://github.com/hbatagelo/abcg/blob/main/LICENSE) for more information.
