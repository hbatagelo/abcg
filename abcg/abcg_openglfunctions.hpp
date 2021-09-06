/**
 * @file abcg_openglfunctions.hpp
 * @brief Declaration of OpenGL-related error checking functions.
 *
 * Error checking wrappers for OpenGL functions are defined here as inline
 * functions.
 *
 * This project is released under the MIT License.
 */

#ifndef ABCG_OPENGLFUNCTIONS_HPP_
#define ABCG_OPENGLFUNCTIONS_HPP_

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
#include <experimental/source_location>
#endif

#include <string_view>

#include "abcg_external.hpp"

namespace abcg {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
using sl = std::experimental::source_location;

void checkGLError(const sl& sourceLocation, std::string_view prefix);

/**
 * @brief Check for OpenGL errors before and after a function call.
 *
 * @tparam TFun Function typename.
 * @tparam TArgs Variadic arguments typename.
 * @param sourceLocation Information about the source code, used for logging.
 * @param function Function to be called.
 * @param args Variadic template arguments for the function.
 * @return Value returned from function, or void.
 */
template <typename TFun, typename... TArgs>
auto callGL(const sl& sourceLocation, TFun&& function, TArgs&&... args) {
  checkGLError(sourceLocation, "BEFORE function call");
  if constexpr (!std::is_void<
                    typename std::result_of<TFun(TArgs...)>::type>::value) {
    // Specialization for functions that do not return void
    auto&& res = std::forward<TFun>(function)(std::forward<TArgs>(args)...);
    checkGLError(sourceLocation, "AFTER function call");
    return res;
  }
  // Specialization for functions that return void
  std::forward<TFun>(function)(std::forward<TArgs>(args)...);
  checkGLError(sourceLocation, "AFTER function call");
}

#else

struct sl {
  static constexpr sl current() noexcept { return sl{}; }
};

/**
 * @brief Call given function with given arguments.
 *
 * @tparam TFun Function typename.
 * @tparam TArgs Variadic arguments typename.
 * @param function Function to be called.
 * @param args Variadic template arguments for the function.
 * @return Value returned from function, or void.
 */
template <typename TFun, typename... TArgs>
auto callGL([[maybe_unused]] sl /*unused*/, TFun&& function, TArgs&&... args) {
  if constexpr (!std::is_void<
                    typename std::result_of<TFun(TArgs...)>::type>::value) {
    // Specialization for functions that do not return void
    auto&& res = std::forward<TFun>(function)(std::forward<TArgs>(args)...);
    return res;
  }
  // Specialization for functions that return void
  std::forward<TFun>(function)(std::forward<TArgs>(args)...);
}
#endif

// OpenGL ES 2.0 function definitions

inline void glActiveTexture(GLenum texture,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glActiveTexture, texture);
}
inline void glAttachShader(GLuint program, GLuint shader,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glAttachShader, program, shader);
}
inline void glBindAttribLocation(GLuint program, GLuint index,
                                 const GLchar* name,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindAttribLocation, program, index, name);
}
inline void glBindBuffer(GLenum target, GLuint buffer,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindBuffer, target, buffer);
}
inline void glBindFramebuffer(GLenum target, GLuint framebuffer,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindFramebuffer, target, framebuffer);
}
inline void glBindRenderbuffer(GLenum target, GLuint renderbuffer,
                               const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindRenderbuffer, target, renderbuffer);
}
inline void glBindTexture(GLenum target, GLuint texture,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindTexture, target, texture);
}
inline void glBlendColor(GLfloat red, GLfloat green, GLfloat blue,
                         GLfloat alpha,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBlendColor, red, green, blue, alpha);
}
inline void glBlendEquation(GLenum mode,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBlendEquation, mode);
}
inline void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha,
                                    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBlendEquationSeparate, modeRGB, modeAlpha);
}
inline void glBlendFunc(GLenum sfactor, GLenum dfactor,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBlendFunc, sfactor, dfactor);
}
inline void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha,
                                GLenum dstAlpha,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBlendFuncSeparate, srcRGB, dstRGB, srcAlpha,
         dstAlpha);
}
inline void glBufferData(GLenum target, GLsizeiptr size, const void* data,
                         GLenum usage,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBufferData, target, size, data, usage);
}
inline void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size,
                            const void* data,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBufferSubData, target, offset, size, data);
}
inline GLenum glCheckFramebufferStatus(
    GLenum target, const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glCheckFramebufferStatus, target);
}
inline void glClear(GLbitfield mask, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glClear, mask);
}
inline void glClearColor(GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glClearColor, red, green, blue, alpha);
}
inline void glClearDepthf(GLfloat d, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glClearDepthf, d);
}
inline void glClearStencil(GLint s, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glClearStencil, s);
}
inline void glColorMask(GLboolean red, GLboolean green, GLboolean blue,
                        GLboolean alpha,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glColorMask, red, green, blue, alpha);
}
inline void glCompileShader(GLuint shader,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glCompileShader, shader);
}
inline void glCompressedTexImage2D(GLenum target, GLint level,
                                   GLenum internalformat, GLsizei width,
                                   GLsizei height, GLint border,
                                   GLsizei imageSize, const void* data,
                                   const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glCompressedTexImage2D, target, level,
         internalformat, width, height, border, imageSize, data);
}
inline void glCompressedTexSubImage2D(
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
    GLsizei height, GLenum format, GLsizei imageSize, const void* data,
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glCompressedTexSubImage2D, target, level, xoffset,
         yoffset, width, height, format, imageSize, data);
}
inline void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat,
                             GLint x, GLint y, GLsizei width, GLsizei height,
                             GLint border,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glCopyTexImage2D, target, level, internalformat, x,
         y, width, height, border);
}
inline void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset,
                                GLint yoffset, GLint x, GLint y, GLsizei width,
                                GLsizei height,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glCopyTexSubImage2D, target, level, xoffset, yoffset,
         x, y, width, height);
}
inline GLuint glCreateProgram(const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glCreateProgram);
}
inline GLuint glCreateShader(GLenum shaderType,
                             const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glCreateShader, shaderType);
}
inline void glCullFace(GLenum mode, const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glCullFace, mode);
}
inline void glDeleteBuffers(GLsizei n, const GLuint* buffers,
                            const sl& sourceLocation = sl::current()) {
  if (buffers == nullptr || *buffers == 0) return;
  callGL(sourceLocation, ::glDeleteBuffers, n, buffers);
}
inline void glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers,
                                 const sl& sourceLocation = sl::current()) {
  if (framebuffers == nullptr || *framebuffers == 0) return;
  callGL(sourceLocation, ::glDeleteFramebuffers, n, framebuffers);
}
inline void glDeleteProgram(GLuint program,
                            const sl& sourceLocation = sl::current()) {
  if (program == 0) return;
  callGL(sourceLocation, ::glDeleteProgram, program);
}
inline void glDeleteRenderbuffers(GLsizei n, GLuint* renderbuffers,
                                  const sl& sourceLocation = sl::current()) {
  if (renderbuffers == nullptr || *renderbuffers == 0) return;
  callGL(sourceLocation, ::glDeleteRenderbuffers, n, renderbuffers);
}
inline void glDeleteShader(GLuint shader,
                           const sl& sourceLocation = sl::current()) {
  if (shader == 0) return;
  callGL(sourceLocation, ::glDeleteShader, shader);
}
inline void glDeleteTextures(GLsizei n, const GLuint* textures,
                             const sl& sourceLocation = sl::current()) {
  if (textures == nullptr || *textures == 0) return;
  callGL(sourceLocation, ::glDeleteTextures, n, textures);
}
inline void glDepthFunc(GLenum func, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDepthFunc, func);
}
inline void glDepthMask(GLboolean flag,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDepthMask, flag);
}
inline void glDepthRangef(GLfloat n, GLfloat f,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDepthRangef, n, f);
}
inline void glDetachShader(GLuint program, GLuint shader,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDetachShader, program, shader);
}
inline void glDisable(GLenum cap, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDisable, cap);
}
inline void glDisableVertexAttribArray(
    GLuint index, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDisableVertexAttribArray, index);
}
inline void glDrawArrays(GLenum mode, GLint first, GLsizei count,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDrawArrays, mode, first, count);
}
inline void glDrawElements(GLenum mode, GLsizei count, GLenum type,
                           const void* indices,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDrawElements, mode, count, type, indices);
}
inline void glEnable(GLenum cap, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glEnable, cap);
}
inline void glEnableVertexAttribArray(
    GLuint index, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glEnableVertexAttribArray, index);
}
inline void glFinish(const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glFinish);
}
inline void glFlush(const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glFlush);
}
inline void glFramebufferRenderbuffer(
    GLenum target, GLenum attachment, GLenum renderbuffertarget,
    GLuint renderbuffer, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glFramebufferRenderbuffer, target, attachment,
         renderbuffertarget, renderbuffer);
}
inline void glFramebufferTexture2D(GLenum target, GLenum attachment,
                                   GLenum textarget, GLuint texture,
                                   GLint level,
                                   const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glFramebufferTexture2D, target, attachment,
         textarget, texture, level);
}
inline void glFrontFace(GLenum mode, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glFrontFace, mode);
}
inline void glGenBuffers(GLsizei n, GLuint* buffers,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenBuffers, n, buffers);
}
inline void glGenerateMipmap(GLenum target,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenerateMipmap, target);
}
inline void glGenFramebuffers(GLsizei n, GLuint* ids,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenFramebuffers, n, ids);
}
inline void glGenRenderbuffers(GLsizei n, GLuint* renderbuffers,
                               const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenRenderbuffers, n, renderbuffers);
}
inline void glGenTextures(GLsizei n, GLuint* textures,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenTextures, n, textures);
}
inline void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize,
                              GLsizei* length, GLint* size, GLenum* type,
                              GLchar* name,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetActiveAttrib, program, index, bufSize, length,
         size, type, name);
}
inline void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize,
                               GLsizei* length, GLint* size, GLenum* type,
                               GLchar* name,
                               const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetActiveUniform, program, index, bufSize, length,
         size, type, name);
}
inline void glGetAttachedShaders(GLuint program, GLsizei maxCount,
                                 GLsizei* count, GLuint* shaders,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetAttachedShaders, program, maxCount, count,
         shaders);
}
inline GLint glGetAttribLocation(GLuint program, const GLchar* name,
                                 const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glGetAttribLocation, program, name);
}
inline void glGetBooleanv(GLenum pname, GLboolean* params,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetBooleanv, pname, params);
}
inline void glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params,
                                   const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetBufferParameteriv, target, pname, params);
}
inline void glGetFloatv(GLenum pname, GLfloat* params,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetFloatv, pname, params);
}
inline void glGetFramebufferAttachmentParameteriv(
    GLenum target, GLenum attachment, GLenum pname, GLint* params,
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetFramebufferAttachmentParameteriv, target,
         attachment, pname, params);
}
inline void glGetIntegerv(GLenum pname, GLint* params,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetIntegerv, pname, params);
}
inline void glGetProgramiv(GLuint program, GLenum pname, GLint* params,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetProgramiv, program, pname, params);
}
inline void glGetProgramInfoLog(GLuint program, GLsizei bufSize,
                                GLsizei* length, GLchar* infoLog,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetProgramInfoLog, program, bufSize, length,
         infoLog);
}
inline void glGetRenderbufferParameteriv(
    GLenum target, GLenum pname, GLint* params,
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetRenderbufferParameteriv, target, pname, params);
}
inline void glGetShaderiv(GLuint shader, GLenum pname, GLint* params,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetShaderiv, shader, pname, params);
}
inline void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length,
                               GLchar* infoLog,
                               const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetShaderInfoLog, shader, bufSize, length,
         infoLog);
}
inline void glGetShaderPrecisionFormat(
    GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision,
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetShaderPrecisionFormat, shadertype,
         precisiontype, range, precision);
}
inline void glGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei* length,
                              GLchar* source,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetShaderSource, shader, bufSize, length, source);
}
inline const GLubyte* glGetString(GLenum name,
                                  const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glGetString, name);
}
inline void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetTexParameterfv, target, pname, params);
}
inline void glGetTexParameteriv(GLenum target, GLenum pname, GLint* params,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetTexParameteriv, target, pname, params);
}
inline void glGetUniformfv(GLuint program, GLint location, GLfloat* params,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetUniformfv, program, location, params);
}
inline void glGetUniformiv(GLuint program, GLint location, GLint* params,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetUniformiv, program, location, params);
}
inline GLint glGetUniformLocation(GLuint program, const GLchar* name,
                                  const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glGetUniformLocation, program, name);
}
inline void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetVertexAttribfv, index, pname, params);
}
inline void glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetVertexAttribiv, index, pname, params);
}
inline void glGetVertexAttribPointerv(
    GLuint index, GLenum pname, void** pointer,
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetVertexAttribPointerv, index, pname, pointer);
}
inline void glHint(GLenum target, GLenum mode,
                   const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glHint, target, mode);
}
inline GLboolean glIsBuffer(GLuint buffer,
                            const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsBuffer, buffer);
}
inline GLboolean glIsEnabled(GLenum cap,
                             const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsEnabled, cap);
}
inline GLboolean glIsFramebuffer(GLuint framebuffer,
                                 const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsFramebuffer, framebuffer);
}
inline GLboolean glIsProgram(GLuint program,
                             const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsProgram, program);
}
inline GLboolean glIsRenderbuffer(GLuint renderbuffer,
                                  const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsRenderbuffer, renderbuffer);
}
inline GLboolean glIsShader(GLuint shader,
                            const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsShader, shader);
}
inline GLboolean glIsTexture(GLuint texture,
                             const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsTexture, texture);
}
inline void glLineWidth(GLfloat width,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glLineWidth, width);
}
inline void glLinkProgram(GLuint program,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glLinkProgram, program);
}
inline void glPixelStorei(GLenum pname, GLint param,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glPixelStorei, pname, param);
}
inline void glPolygonOffset(GLfloat factor, GLfloat units,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glPolygonOffset, factor, units);
}
inline void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, void* pixels,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glReadPixels, x, y, width, height, format, type,
         pixels);
}
inline void glReleaseShaderCompiler(const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glReleaseShaderCompiler);
}
inline void glRenderbufferStorage(GLenum target, GLenum internalformat,
                                  GLsizei width, GLsizei height,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glRenderbufferStorage, target, internalformat, width,
         height);
}
inline void glSampleCoverage(GLfloat value, GLboolean invert,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glSampleCoverage, value, invert);
}
inline void glScissor(GLint x, GLint y, GLsizei width, GLsizei height,
                      const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glScissor, x, y, width, height);
}
inline void glShaderBinary(GLsizei count, const GLuint* shaders,
                           GLenum binaryformat, const void* binary,
                           GLsizei length,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glShaderBinary, count, shaders, binaryformat, binary,
         length);
}
inline void glShaderSource(GLuint shader, GLsizei count, const GLchar** string,
                           const GLint* length,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glShaderSource, shader, count, string, length);
}
inline void glStencilFunc(GLenum func, GLint ref, GLuint mask,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glStencilFunc, func, ref, mask);
}
inline void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref,
                                  GLuint mask,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glStencilFuncSeparate, face, func, ref, mask);
}
inline void glStencilMask(GLuint mask,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glStencilMask, mask);
}
inline void glStencilMaskSeparate(GLenum face, GLuint mask,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glStencilMaskSeparate, face, mask);
}
inline void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glStencilOp, fail, zfail, zpass);
}
inline void glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail,
                                GLenum dppass,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glStencilOpSeparate, face, sfail, dpfail, dppass);
}
inline void glTexImage2D(GLenum target, GLint level, GLint internalformat,
                         GLsizei width, GLsizei height, GLint border,
                         GLenum format, GLenum type, const void* data,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexImage2D, target, level, internalformat, width,
         height, border, format, type, data);
}

inline void glTexParameterf(GLenum target, GLenum pname, GLfloat param,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexParameterf, target, pname, param);
}
inline void glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexParameterfv, target, pname, params);
}
inline void glTexParameteri(GLenum target, GLenum pname, GLint param,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexParameteri, target, pname, param);
}
inline void glTexParameteriv(GLenum target, GLenum pname, const GLint* params,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexParameteriv, target, pname, params);
}
inline void glTexSubImage2D(GLenum target, GLint level, GLint xoffset,
                            GLint yoffset, GLsizei width, GLsizei height,
                            GLenum format, GLenum type, const void* pixels,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexSubImage2D, target, level, xoffset, yoffset,
         width, height, format, type, pixels);
}
inline void glUniform1f(GLint location, GLfloat v0,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform1f, location, v0);
}
inline void glUniform1fv(GLint location, GLsizei count, const GLfloat* value,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform1fv, location, count, value);
}
inline void glUniform1i(GLint location, GLint v0,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform1i, location, v0);
}
inline void glUniform1iv(GLint location, GLsizei count, const GLint* value,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform1iv, location, count, value);
}
inline void glUniform2f(GLint location, GLfloat v0, GLfloat v1,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform2f, location, v0, v1);
}
inline void glUniform2fv(GLint location, GLsizei count, const GLfloat* value,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform2fv, location, count, value);
}
inline void glUniform2i(GLint location, GLint v0, GLint v1,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform2i, location, v0, v1);
}
inline void glUniform2iv(GLint location, GLsizei count, const GLint* value,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform2iv, location, count, value);
}
inline void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform3f, location, v0, v1, v2);
}
inline void glUniform3fv(GLint location, GLsizei count, const GLfloat* value,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform3fv, location, count, value);
}
inline void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform3i, location, v0, v1, v2);
}
inline void glUniform3iv(GLint location, GLsizei count, const GLint* value,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform3iv, location, count, value);
}
inline void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2,
                        GLfloat v3, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform4f, location, v0, v1, v2, v3);
}
inline void glUniform4fv(GLint location, GLsizei count, const GLfloat* value,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform4fv, location, count, value);
}
inline void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform4i, location, v0, v1, v2, v3);
}
inline void glUniform4iv(GLint location, GLsizei count, const GLint* value,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform4iv, location, count, value);
}
inline void glUniformMatrix2fv(GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value,
                               const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformMatrix2fv, location, count, transpose,
         value);
}
inline void glUniformMatrix3fv(GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value,
                               const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformMatrix3fv, location, count, transpose,
         value);
}
inline void glUniformMatrix4fv(GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value,
                               const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformMatrix4fv, location, count, transpose,
         value);
}
inline void glUseProgram(GLuint program,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUseProgram, program);
}
inline void glValidateProgram(GLuint program,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glValidateProgram, program);
}
inline void glVertexAttrib1f(GLuint index, GLfloat x,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttrib1f, index, x);
}
inline void glVertexAttrib1fv(GLuint index, const GLfloat* v,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttrib1fv, index, v);
}
inline void glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttrib2f, index, x, y);
}
inline void glVertexAttrib2fv(GLuint index, const GLfloat* v,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttrib2fv, index, v);
}
inline void glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttrib3f, index, x, y, z);
}
inline void glVertexAttrib3fv(GLuint index, const GLfloat* v,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttrib3fv, index, v);
}
inline void glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z,
                             GLfloat w,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttrib4f, index, x, y, z, w);
}
inline void glVertexAttrib4fv(GLuint index, const GLfloat* v,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttrib4fv, index, v);
}
inline void glVertexAttribPointer(GLuint index, GLint size, GLenum type,
                                  GLboolean normalized, GLsizei stride,
                                  const void* pointer,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttribPointer, index, size, type, normalized,
         stride, pointer);
}
inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height,
                       const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glViewport, x, y, width, height);
}

// OpenGL ES 3.0 function definitions

inline void glReadBuffer(GLenum src, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glReadBuffer, src);
}
inline void glDrawRangeElements(GLenum mode, GLuint start, GLuint end,
                                GLsizei count, GLenum type, const void* indices,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDrawRangeElements, mode, start, end, count, type,
         indices);
}
inline void glTexImage3D(GLenum target, GLint level, GLint internalformat,
                         GLsizei width, GLsizei height, GLsizei depth,
                         GLint border, GLenum format, GLenum type,
                         const void* pixels,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexImage3D, target, level, internalformat, width,
         height, depth, border, format, type, pixels);
}
inline void glTexSubImage3D(GLenum target, GLint level, GLint xoffset,
                            GLint yoffset, GLint zoffset, GLsizei width,
                            GLsizei height, GLsizei depth, GLenum format,
                            GLenum type, const void* pixels,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexSubImage3D, target, level, xoffset, yoffset,
         zoffset, width, height, depth, format, type, pixels);
}
inline void glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset,
                                GLint yoffset, GLint zoffset, GLint x, GLint y,
                                GLsizei width, GLsizei height,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glCopyTexSubImage3D, target, level, xoffset, yoffset,
         zoffset, x, y, width, height);
}
inline void glCompressedTexImage3D(GLenum target, GLint level,
                                   GLenum internalformat, GLsizei width,
                                   GLsizei height, GLsizei depth, GLint border,
                                   GLsizei imageSize, const void* data,
                                   const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glCompressedTexImage3D, target, level,
         internalformat, width, height, depth, border, imageSize, data);
}
inline void glCompressedTexSubImage3D(
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format,
    GLsizei imageSize, const void* data,
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glCompressedTexSubImage3D, target, level, xoffset,
         yoffset, zoffset, width, height, depth, format, imageSize, data);
}
inline void glGenQueries(GLsizei n, GLuint* ids,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenQueries, n, ids);
}
inline void glDeleteQueries(GLsizei n, const GLuint* ids,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteQueries, n, ids);
}
inline GLboolean glIsQuery(GLuint id,
                           const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsQuery, id);
}
inline void glBeginQuery(GLenum target, GLuint id,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBeginQuery, target, id);
}
inline void glEndQuery(GLenum target,
                       const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glEndQuery, target);
}
inline void glGetQueryiv(GLenum target, GLenum pname, GLint* params,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetQueryiv, target, pname, params);
}
inline void glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetQueryObjectuiv, id, pname, params);
}
inline GLboolean glUnmapBuffer(GLenum target,
                               const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glUnmapBuffer, target);
}
inline void glGetBufferPointerv(GLenum target, GLenum pname, void** params,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetBufferPointerv, target, pname, params);
}
inline void glDrawBuffers(GLsizei n, const GLenum* bufs,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDrawBuffers, n, bufs);
}
inline void glUniformMatrix2x3fv(GLint location, GLsizei count,
                                 GLboolean transpose, const GLfloat* value,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformMatrix2x3fv, location, count, transpose,
         value);
}
inline void glUniformMatrix3x2fv(GLint location, GLsizei count,
                                 GLboolean transpose, const GLfloat* value,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformMatrix3x2fv, location, count, transpose,
         value);
}
inline void glUniformMatrix2x4fv(GLint location, GLsizei count,
                                 GLboolean transpose, const GLfloat* value,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformMatrix2x4fv, location, count, transpose,
         value);
}
inline void glUniformMatrix4x2fv(GLint location, GLsizei count,
                                 GLboolean transpose, const GLfloat* value,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformMatrix4x2fv, location, count, transpose,
         value);
}
inline void glUniformMatrix3x4fv(GLint location, GLsizei count,
                                 GLboolean transpose, const GLfloat* value,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformMatrix3x4fv, location, count, transpose,
         value);
}
inline void glUniformMatrix4x3fv(GLint location, GLsizei count,
                                 GLboolean transpose, const GLfloat* value,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformMatrix4x3fv, location, count, transpose,
         value);
}
inline void glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1,
                              GLint srcY1, GLint dstX0, GLint dstY0,
                              GLint dstX1, GLint dstY1, GLbitfield mask,
                              GLenum filter,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBlitFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0,
         dstY0, dstX1, dstY1, mask, filter);
}
inline void glRenderbufferStorageMultisample(
    GLenum target, GLsizei samples, GLenum internalformat, GLsizei width,
    GLsizei height, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glRenderbufferStorageMultisample, target, samples,
         internalformat, width, height);
}
inline void glFramebufferTextureLayer(
    GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer,
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glFramebufferTextureLayer, target, attachment,
         texture, level, layer);
}
inline void* glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length,
                              GLbitfield access,
                              const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glMapBufferRange, target, offset, length,
                access);
}
inline void glFlushMappedBufferRange(GLenum target, GLintptr offset,
                                     GLsizeiptr length,
                                     const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glFlushMappedBufferRange, target, offset, length);
}
inline void glBindVertexArray(GLuint array,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindVertexArray, array);
}
inline void glDeleteVertexArrays(GLsizei n, const GLuint* arrays,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteVertexArrays, n, arrays);
}
inline void glGenVertexArrays(GLsizei n, GLuint* arrays,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenVertexArrays, n, arrays);
}
inline GLboolean glIsVertexArray(GLuint array,
                                 const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsVertexArray, array);
}
inline void glGetIntegeri_v(GLenum target, GLuint index, GLint* data,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetIntegeri_v, target, index, data);
}
inline void glBeginTransformFeedback(GLenum primitiveMode,
                                     const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBeginTransformFeedback, primitiveMode);
}
inline void glEndTransformFeedback(const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glEndTransformFeedback);
}
inline void glBindBufferRange(GLenum target, GLuint index, GLuint buffer,
                              GLintptr offset, GLsizeiptr size,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindBufferRange, target, index, buffer, offset,
         size);
}
inline void glBindBufferBase(GLenum target, GLuint index, GLuint buffer,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindBufferBase, target, index, buffer);
}
inline void glTransformFeedbackVaryings(
    GLuint program, GLsizei count, const GLchar* const* varyings,
    GLenum bufferMode, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTransformFeedbackVaryings, program, count,
         varyings, bufferMode);
}
inline void glGetTransformFeedbackVarying(
    GLuint program, GLuint index, GLsizei bufSize, GLsizei* length,
    GLsizei* size, GLenum* type, GLchar* name,
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetTransformFeedbackVarying, program, index,
         bufSize, length, size, type, name);
}
inline void glVertexAttribIPointer(GLuint index, GLint size, GLenum type,
                                   GLsizei stride, const void* pointer,
                                   const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttribIPointer, index, size, type, stride,
         pointer);
}
inline void glGetVertexAttribIiv(GLuint index, GLenum pname, GLint* params,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetVertexAttribIiv, index, pname, params);
}
inline void glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint* params,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetVertexAttribIuiv, index, pname, params);
}
inline void glVertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttribI4i, index, x, y, z, w);
}
inline void glVertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z,
                               GLuint w,
                               const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttribI4ui, index, x, y, z, w);
}
inline void glVertexAttribI4iv(GLuint index, const GLint* v,
                               const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttribI4iv, index, v);
}
inline void glVertexAttribI4uiv(GLuint index, const GLuint* v,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttribI4uiv, index, v);
}
inline void glGetUniformuiv(GLuint program, GLint location, GLuint* params,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetUniformuiv, program, location, params);
}
inline GLint glGetFragDataLocation(GLuint program, const GLchar* name,
                                   const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glGetFragDataLocation, program, name);
}
inline void glUniform1ui(GLint location, GLuint v0,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform1ui, location, v0);
}
inline void glUniform2ui(GLint location, GLuint v0, GLuint v1,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform2ui, location, v0, v1);
}
inline void glUniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform3ui, location, v0, v1, v2);
}
inline void glUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2,
                         GLuint v3, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform4ui, location, v0, v1, v2, v3);
}
inline void glUniform1uiv(GLint location, GLsizei count, const GLuint* value,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform1uiv, location, count, value);
}
inline void glUniform2uiv(GLint location, GLsizei count, const GLuint* value,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform2uiv, location, count, value);
}
inline void glUniform3uiv(GLint location, GLsizei count, const GLuint* value,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform3uiv, location, count, value);
}
inline void glUniform4uiv(GLint location, GLsizei count, const GLuint* value,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform4uiv, location, count, value);
}
inline void glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glClearBufferiv, buffer, drawbuffer, value);
}
inline void glClearBufferuiv(GLenum buffer, GLint drawbuffer,
                             const GLuint* value,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glClearBufferuiv, buffer, drawbuffer, value);
}
inline void glClearBufferfv(GLenum buffer, GLint drawbuffer,
                            const GLfloat* value,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glClearBufferfv, buffer, drawbuffer, value);
}
inline void glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth,
                            GLint stencil,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glClearBufferfi, buffer, drawbuffer, depth, stencil);
}
inline const GLubyte* glGetStringi(GLenum name, GLuint index,
                                   const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glGetStringi, name, index);
}
inline void glCopyBufferSubData(GLenum readTarget, GLenum writeTarget,
                                GLintptr readOffset, GLintptr writeOffset,
                                GLsizeiptr size,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glCopyBufferSubData, readTarget, writeTarget,
         readOffset, writeOffset, size);
}
inline void glGetUniformIndices(GLuint program, GLsizei uniformCount,
                                const GLchar* const* uniformNames,
                                GLuint* uniformIndices,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetUniformIndices, program, uniformCount,
         uniformNames, uniformIndices);
}
inline void glGetActiveUniformsiv(GLuint program, GLsizei uniformCount,
                                  const GLuint* uniformIndices, GLenum pname,
                                  GLint* params,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetActiveUniformsiv, program, uniformCount,
         uniformIndices, pname, params);
}
inline GLuint glGetUniformBlockIndex(GLuint program,
                                     const GLchar* uniformBlockName,
                                     const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glGetUniformBlockIndex, program,
                uniformBlockName);
}
inline void glGetActiveUniformBlockiv(
    GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params,
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetActiveUniformBlockiv, program,
         uniformBlockIndex, pname, params);
}
inline void glGetActiveUniformBlockName(
    GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length,
    GLchar* uniformBlockName, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetActiveUniformBlockName, program,
         uniformBlockIndex, bufSize, length, uniformBlockName);
}
inline void glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex,
                                  GLuint uniformBlockBinding,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformBlockBinding, program, uniformBlockIndex,
         uniformBlockBinding);
}

inline void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count,
                                  GLsizei instancecount,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDrawArraysInstanced, mode, first, count,
         instancecount);
}
inline void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type,
                                    const void* indices, GLsizei instancecount,
                                    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDrawElementsInstanced, mode, count, type, indices,
         instancecount);
}
inline GLsync glFenceSync(GLenum condition, GLbitfield flags,
                          const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glFenceSync, condition, flags);
}
inline GLboolean glIsSync(GLsync sync,
                          const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsSync, sync);
}
inline void glDeleteSync(GLsync sync,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteSync, sync);
}
inline GLenum glClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout,
                               const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glClientWaitSync, sync, flags, timeout);
}
inline void glWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout,
                       const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glWaitSync, sync, flags, timeout);
}
inline void glGetInteger64v(GLenum pname, GLint64* data,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetInteger64v, pname, data);
}
inline void glGetSynciv(GLsync sync, GLenum pname, GLsizei count,
                        GLsizei* length, GLint* values,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetSynciv, sync, pname, count, length, values);
}
inline void glGetInteger64i_v(GLenum target, GLuint index, GLint64* data,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetInteger64i_v, target, index, data);
}
inline void glGetBufferParameteri64v(GLenum target, GLenum pname,
                                     GLint64* params,
                                     const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetBufferParameteri64v, target, pname, params);
}
inline void glGenSamplers(GLsizei count, GLuint* samplers,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenSamplers, count, samplers);
}
inline void glDeleteSamplers(GLsizei count, const GLuint* samplers,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteSamplers, count, samplers);
}
inline GLboolean glIsSampler(GLuint sampler,
                             const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsSampler, sampler);
}
inline void glBindSampler(GLuint unit, GLuint sampler,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindSampler, unit, sampler);
}
inline void glSamplerParameteri(GLuint sampler, GLenum pname, GLint param,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glSamplerParameteri, sampler, pname, param);
}
inline void glSamplerParameteriv(GLuint sampler, GLenum pname,
                                 const GLint* param,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glSamplerParameteriv, sampler, pname, param);
}
inline void glSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glSamplerParameterf, sampler, pname, param);
}
inline void glSamplerParameterfv(GLuint sampler, GLenum pname,
                                 const GLfloat* param,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glSamplerParameterfv, sampler, pname, param);
}
inline void glGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint* params,
                                    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetSamplerParameteriv, sampler, pname, params);
}
inline void glGetSamplerParameterfv(GLuint sampler, GLenum pname,
                                    GLfloat* params,
                                    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetSamplerParameterfv, sampler, pname, params);
}
inline void glVertexAttribDivisor(GLuint index, GLuint divisor,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glVertexAttribDivisor, index, divisor);
}
inline void glBindTransformFeedback(GLenum target, GLuint id,
                                    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindTransformFeedback, target, id);
}
inline void glDeleteTransformFeedbacks(
    GLsizei n, const GLuint* ids, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteTransformFeedbacks, n, ids);
}
inline void glGenTransformFeedbacks(GLsizei n, GLuint* ids,
                                    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenTransformFeedbacks, n, ids);
}
inline GLboolean glIsTransformFeedback(
    GLuint id, const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glIsTransformFeedback, id);
}
inline void glPauseTransformFeedback(const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glPauseTransformFeedback);
}
inline void glResumeTransformFeedback(
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glResumeTransformFeedback);
}
inline void glGetProgramBinary(GLuint program, GLsizei bufSize, GLsizei* length,
                               GLenum* binaryFormat, void* binary,
                               const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetProgramBinary, program, bufSize, length,
         binaryFormat, binary);
}
inline void glProgramBinary(GLuint program, GLenum binaryFormat,
                            const void* binary, GLsizei length,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glProgramBinary, program, binaryFormat, binary,
         length);
}
inline void glProgramParameteri(GLuint program, GLenum pname, GLint value,
                                const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glProgramParameteri, program, pname, value);
}
inline void glInvalidateFramebuffer(GLenum target, GLsizei numAttachments,
                                    const GLenum* attachments,
                                    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glInvalidateFramebuffer, target, numAttachments,
         attachments);
}
inline void glInvalidateSubFramebuffer(
    GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x,
    GLint y, GLsizei width, GLsizei height,
    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glInvalidateSubFramebuffer, target, numAttachments,
         attachments, x, y, width, height);
}
inline void glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat,
                           GLsizei width, GLsizei height,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexStorage2D, target, levels, internalformat,
         width, height);
}
inline void glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat,
                           GLsizei width, GLsizei height, GLsizei depth,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexStorage3D, target, levels, internalformat,
         width, height, depth);
}
inline void glGetInternalformativ(GLenum target, GLenum internalformat,
                                  GLenum pname, GLsizei count, GLint* params,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetInternalformativ, target, internalformat, pname,
         count, params);
}

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)

// OpenGL 3.0+ function definitions

inline void glBindFragDataLocation(GLuint program, GLuint colorNumber,
                                   const char* name,
                                   const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindFragDataLocation, program, colorNumber, name);
}

// OpenGL 3.2+ function definitions
// OpenGL ES 3.2 function definitions

inline void glFramebufferTexture(GLenum target, GLenum attachment,
                                 GLuint texture, GLint level,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glFramebufferTexture, target, attachment, texture,
         level);
}

inline void glTexImage2DMultisample(GLenum target, GLsizei samples,
                                    GLenum internalformat, GLsizei width,
                                    GLsizei height,
                                    GLboolean fixedsamplelocations,
                                    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexImage2DMultisample, target, samples,
         internalformat, width, height, fixedsamplelocations);
}

// OpenGL 2.0+ function definitions

inline void glGetDoublev(GLenum pname, GLdouble* params,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetDoublev, pname, params);
}

#endif

}  // namespace abcg

#endif