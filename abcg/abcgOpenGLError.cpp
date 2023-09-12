/**
 * @file abcgOpenGLError.cpp
 * @brief Definition of abcg::OpenGLError members.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgOpenGLError.hpp"
#include "abcgExternal.hpp"
#include "abcgOpenGLExternal.hpp"
#include "abcgUtil.hpp"

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
/**
 * @brief Constructs an abcg::OpenGLError object.
 *
 * @param what Explanatory string.
 * @param errorCode OpenGL error code, as returned by `glGetError`.
 * @param sourceLocation Information about the source code.
 *
 * The object generates a color-coded string containing the error name, the
 * descriptive error messages of all errors returned by `glGetError`, and the
 * source code information.
 */
abcg::OpenGLError::OpenGLError(std::string_view what, unsigned int errorCode,
                               source_location const &sourceLocation)
    : Exception(prettyPrint(what, errorCode, sourceLocation)) {}

std::string
abcg::OpenGLError::prettyPrint(std::string_view what, unsigned int errorCode,
                               source_location const &sourceLocation) {
  auto errorMessage{toRedString(std::string{"OpenGL error "} + what.data())};
  if (SDL_GL_GetCurrentContext() == nullptr) {
    errorMessage += " (invalid OpenGL context)";
  } else {
    auto appendGLErrorString{[&errorMessage, &errorCode]() {
      errorMessage += fmt::format(" ({})", getGLErrorString(errorCode).data());
    }};
    appendGLErrorString();
    //  Clear remaining errors
    while (glGetError() != GL_NO_ERROR) {
      appendGLErrorString();
    }
  }
  return errorMessage + " in " + sourceLocation.file_name() + ":" +
         std::to_string(sourceLocation.line()) + ", " +
         toYellowString(sourceLocation.function_name()) + "\n";
}
#else
abcg::OpenGLError::OpenGLError(std::string_view what, unsigned int errorCode)
    : Exception(prettyPrint(what, errorCode)) {}

std::string abcg::OpenGLError::prettyPrint(std::string_view what,
                                           unsigned int errorCode) {
  auto errorMessage{toRedString(std::string{"OpenGL error "} + what.data())};
  if (SDL_GL_GetCurrentContext() == nullptr) {
    errorMessage += " (invalid OpenGL context)";
  } else {
    auto appendGLErrorString{[&errorMessage, &errorCode]() {
      errorMessage += fmt::format(" ({})", getGLErrorString(errorCode).data());
    }};
    appendGLErrorString();
    //  Clear remaining errors
    while (glGetError() != GL_NO_ERROR) {
      appendGLErrorString();
    }
  }
  return errorMessage;
}
#endif

std::string_view abcg::OpenGLError::getGLErrorString(unsigned int errorCode) {
  using namespace std::string_literals;
  std::string_view result;
  switch (errorCode) {
  case GL_NO_ERROR: // 0
    result = "GL_NO_ERROR: no error";
    break;
  case GL_INVALID_ENUM: // 0x0500
    result = "GL_INVALID_ENUM: GLenum argument out of range";
    break;
  case GL_INVALID_VALUE: // 0x0501
    result = "GL_INVALID_VALUE: numeric argument out of range";
    break;
  case GL_INVALID_OPERATION: // 0x0502
    result = "GL_INVALID_OPERATION: operation illegal in current state";
    break;
  case GL_OUT_OF_MEMORY: // 0x0505
    result = "GL_OUT_OF_MEMORY: not enough memory left to execute function";
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION: // 0x0506
    result = "GL_INVALID_FRAMEBUFFER_OPERATION: cannot read from or write to a "
             "framebuffer that is not complete";
    break;
  case GL_FRAMEBUFFER_UNDEFINED: // 0x8219
    result = "GL_FRAMEBUFFER_UNDEFINED: default framebuffer does not exist";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: // 0x8CD6
    result = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: any of the framebuffer "
             "attachment points are framebuffer "
             "incomplete";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: // 0x8CD7
    result = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: framebuffer does "
             "not have at least one image attached to it";
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED: // 0x8CDD
    result = "GL_FRAMEBUFFER_UNSUPPORTED: depth and stencil attachments, if "
             "present, are not the same "
             "renderbuffer, or if the combination of internal formats of the "
             "attached images violates an implementation-dependent set of "
             "restrictions";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: // 0x8D56
    result = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: value of "
             "GL_RENDERBUFFER_SAMPLES is not the same for all attached "
             "renderbuffers or, if the attached images are a mix of "
             "renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES "
             "is not zero";
    break;
#if !defined(__EMSCRIPTEN__)
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: // 0x8CDB
    result = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: value of "
             "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for "
             "color attachment point(s) named by GL_DRAW_BUFFERi";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: // 0x8CDC
    result = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: GL_READ_BUFFER is not "
             "GL_NONE and the value of "
             "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color "
             "attachment point named by GL_READ_BUFFER";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: // 0x8CA8
    result =
        "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: a framebuffer attachment "
        "is layered and a populated attachment is "
        "not layered, or all populated color attachments are not from "
        "textures of the same target";
    break;
#endif
#if defined(GL_KHR_debug) && !defined(__EMSCRIPTEN__)
  case GL_STACK_OVERFLOW: // 0x0503
    result = "GL_STACK_OVERFLOW: function would cause a stack overflow";
    break;
  case GL_STACK_UNDERFLOW: // 0x0504
    result = "GL_STACK_UNDERFLOW: function would cause a stack underflow";
    break;
  case GL_CONTEXT_LOST: // 0x0507
    result = "GL_CONTEXT_LOST: context lost due to a graphics card reset";
    break;
#endif
  default:
    result = "Unknown error";
  }
  return result;
}
