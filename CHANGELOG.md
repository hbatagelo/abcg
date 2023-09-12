# Release notes

## v3.1.0

*   Use extra stack space when building for WASM.
*   Updated external libraries (Dear ImGui v1.89.4; {fmt} 10.0.0; volk 1.3.250).
*   Force flush to stdout after each invocation of fmt::print to make it work with MSYS2.
*   `abcg::flipHorizontally` and `abcg::flipVertically` now accepts an l-value reference to a SDL surface instead of a non-null pointer.
*   Moved `abcg::toRedString`, `abcg::toYellowString` and `abcg::toBlueString` to `abcgUtil.hpp`.
*   General improvements (fixed typos, code style, etc).

## v3.0.0

### New features

*   Added support for building in Visual Studio with MSVC. For that to work, GLEW, SDL2 and SDL2\_image development libraries must be installed and set up as follows: the GLEW installation directory must be added to the system \_Path\_ variable; the environment variables `SDL2DIR` and `SDL2IMAGEDIR` must be set to the installation directory of SDL2 and SDL2\_image, respectively. Debugging is also supported. However, **building multiple executables is not supported in VS**. Thus, make sure to use a single `add_subdirectory()` command in the `CMakeLists.txt` of the root directory. Also notice that the default output directory will be `out/build` instead of `build` if the project is built in the IDE.

*   Added support for building with MSVC in VS Code. Multiple executables are supported, as well as debugging. Below is an example of a `launch.json` for debugging the "Hello, World!" project:

        {
        	"version": "0.2.0",
        	"configurations": [
        	{
        		"name": "C++ Launch (Windows)",
        		"type": "cppvsdbg",
        		"request": "launch",
        		"program": "${workspaceFolder}/build/bin/helloworld/helloworld.exe",
        		"symbolSearchPath": "${workspaceFolder}/build/bin/Debug",
        		"console": "integratedTerminal",
        		"args": [],
        		"stopAtEntry": false
        	}
        	]
        }

*   Updated `abcg::createOpenGLProgram` with support for additional shader types and no dependance on `abcg::OpenGLWindow`. The function now accepts an object of type `abcg::ShaderSource` containing the paths or codes of any combination of shaders supported by OpenGL (vertex, fragment, geometry, tess control/evaluation, and compute shaders). The function also accepts a boolean as a second argument to toggle on/off exceptions on build errors. By default, exception throwing is enabled.

*   As an alternative to `abcg::createOpenGLProgram`, it is possible to split the build process into smaller parts by calling `abcg::triggerOpenGLShaderCompile`, `abcg::checkOpenGLShaderCompile`, `abcg::triggerOpenGLShaderLink`, and `abcg::checkOpenGLShaderLink` in sequence. This can be useful to prevent halting the application when building complex programs.

*   Added `abcg::Application::getBasePath` and `abcg::Application::getAssetsPath` as static member functions.

*   The HTML element ID used for registering the fullscreen callback function can now be set with `abcg::WindowSettings::fullscreenElementID` (default is `#canvas`).

*   `abcg::loadOpenGLTexture` and `abcg::loadOpenGLCubemap` now accepts creation info structures `abcg::OpenGLTextureCreateInfo` and `abcg::OpenGLCubemapCreateInfo`, respectively.

*   Added `abcg::hashCombine` and `abcg::hashCombineSeed` functions to easily combine hash values.

*   Added support for Vulkan.

### Breaking changes

*   ABCg filenames changed to camel case.
*   `abcg::OpenGLWindow::getAssetsPath` replaced with `abcg::Application::getAssetsPath`.
*   `abcg::OpenGLWindow::createProgramFromFile` and `abcg::OpenGLWindow::createProgramFromString` removed. Use `abcg::createOpenGLProgram` instead.
*   `abcg::OpenGLWindow::handleEvent` renamed to `abcg::OpenGLWindow::onEvent`.
*   `abcg::OpenGLWindow::initializeGL` renamed to `abcg::OpenGLWindow::onCreate`.
*   `abcg::OpenGLWindow::paintGL` renamed to `abcg::OpenGLWindow::onPaint`.
*   `abcg::OpenGLWindow::paintUI` renamed to `abcg::OpenGLWindow::onPaintUI`.
*   `abcg::OpenGLWindow::resizeGL` renamed to `abcg::OpenGLWindow::onResize`.
*   `abcg::OpenGLWindow::terminateGL` renamed to `abcg::OpenGLWindow::onDestroy`.
*   Namespace `abcg::opengl` removed.
*   `abcg::opengl::loadTexture` renamed to `abcg::loadOpenGLTexture`.
*   `abcg::opengl::loadCubemap` renamed to `abcg::loadOpenGLCubemap`.
*   `abcg::Application` does not take ownership of `abcg::OpenGLWindow` anymore. `abcg::Application::run` now accepts an lvalue reference to `abcg::OpenGLWindow`.
*   `abcg_string.hpp` and `abcg_string.cpp` removed.
*   The static member functions of `abcg::Exception`, namely `Runtime`, `OpenGL`, `SDL`, and `SDLImage`, are now classes of their own: `abcg::RuntimeError`, `abcg::OpenGLError`, `abcg::SDLError`, and `abcg::SDLImageError`. This simplifies the syntax for throwing ABCg exceptions. For instance, `throw abcg::Exception{abcg::Exception::runtime(...)}` now becomes `throw abcg::RuntimeError(...)`.
*   Default value of `abcg::OpenGLSettings::stencilBufferSize` changed from 8 to 0.
*   `abcg::OpenGLSettings::vsync` renamed to `abcg::OpenGLSettings::vSync`.
*   `abcg::OpenGLWindow::onResize` parameters changed from `int width, int height` to `glm::ivec2 size`.

### Other changes

*   Updated external libraries (Dear ImGui v1.86; {fmt} 8.1.1; GSL 4.0.0).
*   Minimum required version for CMake increased to 3.21.
*   `abcg::OpenGLWindow::getDeltaTime()` marked `noexcept`.

## v2.0.1

### Bug Fixes

*   Fixed flickering effect when `glClear` is not called for each frame. For this fix to work, `abcg::OpenGLSettings::preserveWebGLDrawingBuffer` must be `true` even when building for desktop.

## v2.0.0

### Breaking changes

*   `abcg::Application::run` now takes by value a unique pointer to `abcg::OpenGLWindow`. Since `std::unique_ptr` cannot be copied, the caller must either use `std::move` or pass the pointer as an rvalue. This makes clear the intent of transferring ownership of the pointer, which was not explicit in the previous version that takes an lvalue reference.
*   Support for running multiple windows has been dropped. Multiple windows weren't working properly on Emscripten builds and aren't used in the course.
*   `abcg::opengl::loadCubemap` now transforms the cube map textures to a right-handed coordinate system by default. A small loading overhead is incurred because some of the bitmaps are flipped vertically and horizontally. This behavior can be disabled by setting the (new) parameter `rightHandedSystem` to `false`.

### Other changes

*   `abcg::Exception::OpenGL` now generates a string describing all OpenGL error messages returned by `glGetError` when there are multiple errors.
*   All OpenGL functions from OpenGL ES 2.0 and ES 3.0 can now be qualified with the `abcg` namespace (e.g. `abcg::glActiveTexture(0)`) for automatic error handling in debug builds. This is an alternative to the `GL_CHECK` macro used in many engines to check for errors before and after each GL call.
*   Updated external libraries (Dear ImGui v1.84; cppitertools v2.1; {fmt} 8.0.1).

## v1.0.0

*   Initial release, used during the first academic term of 2021.
