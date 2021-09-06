Version history
======

### v2.0.0

#### Breaking changes
* `abcg::Application::run` now takes by value a unique pointer to `abcg::OpenGLWindow`. Since `std::unique_ptr` cannot be copied, the caller must either use `std::move` or pass the pointer as an rvalue. This makes clear the intent of transferring ownership of the pointer, which was not explicit in the previous version that takes an lvalue reference.
* Support for running multiple windows has been dropped. Multiple windows weren't working properly on Emscripten builds and aren't used in the course.

#### Other changes
* `abcg::Exception::OpenGL` now generates a string describing all OpenGL error messages returned by `glGetError` when there are multiple errors.
* All OpenGL functions from OpenGL ES 2.0 and ES 3.0 can now be qualified with the `abcg` namespace (e.g. `abcg::glActiveTexture(0)`) for automatic error handling in debug builds. This is an alternative to the `GL_CHECK` macro used in many engines to check for errors before and after each GL call.
* Now, by default, `abcg::opengl::loadCubemap` transforms the cube map textures to a right-handed coordinate system. This will add a small loading overhead because some of the bitmaps are flipped vertically and horizontally. This behavior can be disabled by setting the (new) parameter `rightHandedSystem` to `false`.
* Updated external libraries (Dear ImGui v1.84; cppitertools v2.1; {fmt} 8.0.1).

### v1.0.0

* Initial release. Used during the first academic term of 2021.
