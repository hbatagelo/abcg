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
void checkGLError(const std::experimental::source_location& sourceLocation,
                  std::string_view prefix);

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
auto callGL(const std::experimental::source_location& sourceLocation,
            TFun&& function, TArgs&&... args) {
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

using sl = std::experimental::source_location;

inline void glActiveTexture(GLenum texture,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glActiveTexture, texture);
}
inline void glAttachShader(GLuint program, GLuint shader,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glAttachShader, program, shader);
}
inline void glBindBuffer(GLenum target, GLuint buffer,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindBuffer, target, buffer);
}
inline void glBindBufferBase(GLenum target, GLuint index, GLuint buffer,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindBufferBase, target, index, buffer);
}
inline void glBindFragDataLocation(GLuint program, GLuint colorNumber,
                                   const char* name,
                                   const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindFragDataLocation, program, colorNumber, name);
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
inline void glBindVertexArray(GLuint array,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBindVertexArray, array);
}
inline void glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1,
                              GLint srcY1, GLint dstX0, GLint dstY0,
                              GLint dstX1, GLint dstY1, GLbitfield mask,
                              GLenum filter,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBlitFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0,
         dstY0, dstX1, dstY1, mask, filter);
}
inline void glBufferData(GLenum target, GLsizeiptr size, const void* data,
                         GLenum usage,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glBufferData, target, size, data, usage);
}
inline void glClear(GLbitfield mask, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glClear, mask);
}
inline void glClearColor(GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glClearColor, red, green, blue, alpha);
}
inline GLuint glCreateProgram(const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glCreateProgram);
}
inline GLuint glCreateShader(GLenum shaderType,
                             const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glCreateShader, shaderType);
}
inline GLenum glCheckFramebufferStatus(
    GLenum target, const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glCheckFramebufferStatus, target);
}
inline void glCompileShader(GLuint shader,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glCompileShader, shader);
}
inline void glDeleteBuffers(GLsizei n, const GLuint* buffers,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteBuffers, n, buffers);
}
inline void glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteFramebuffers, n, framebuffers);
}
inline void glDeleteProgram(GLuint program,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteProgram, program);
}
inline void glDeleteRenderbuffers(GLsizei n, GLuint* renderbuffers,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteRenderbuffers, n, renderbuffers);
}
inline void glDeleteShader(GLuint shader,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteShader, shader);
}
inline void glDeleteTextures(GLsizei n, const GLuint* textures,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteTextures, n, textures);
}
inline void glDeleteVertexArrays(GLsizei n, const GLuint* arrays,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDeleteVertexArrays, n, arrays);
}
inline void glDrawBuffers(GLsizei n, const GLenum* bufs,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDrawBuffers, n, bufs);
}
inline void glDrawElements(GLenum mode, GLsizei count, GLenum type,
                           const void* indices,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDrawElements, mode, count, type, indices);
}
inline void glDrawArrays(GLenum mode, GLint first, GLsizei count,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glDrawArrays, mode, first, count);
}
inline void glEnable(GLenum cap, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glEnable, cap);
}
inline void glEnableVertexAttribArray(
    GLuint index, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glEnableVertexAttribArray, index);
}
inline void glFramebufferRenderbuffer(
    GLenum target, GLenum attachment, GLenum renderbuffertarget,
    GLuint renderbuffer, const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glFramebufferRenderbuffer, target, attachment,
         renderbuffertarget, renderbuffer);
}
inline void glFramebufferTexture(GLenum target, GLenum attachment,
                                 GLuint texture, GLint level,
                                 const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glFramebufferTexture, target, attachment, texture,
         level);
}
inline void glGenerateMipmap(GLenum target,
                             const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenerateMipmap, target);
}
inline void glGenBuffers(GLsizei n, GLuint* buffers,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenBuffers, n, buffers);
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
inline void glGenVertexArrays(GLsizei n, GLuint* arrays,
                              const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGenVertexArrays, n, arrays);
}
inline GLint glGetAttribLocation(GLuint program, const GLchar* name,
                                 const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glGetAttribLocation, program, name);
}
inline void glGetBooleanv(GLenum pname, GLboolean* params,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetBooleanv, pname, params);
}
inline void glGetDoublev(GLenum pname, GLdouble* params,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetDoublev, pname, params);
}
inline void glGetFloatv(GLenum pname, GLfloat* params,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetFloatv, pname, params);
}
inline void glGetIntegerv(GLenum pname, GLint* params,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetIntegerv, pname, params);
}
inline void glGetShaderiv(GLuint shader, GLenum pname, GLint* params,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetShaderiv, shader, pname, params);
}
inline const GLubyte* glGetString(GLenum name,
                                  const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glGetString, name);
}
inline void glGetProgramiv(GLuint program, GLenum pname, GLint* params,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glGetProgramiv, program, pname, params);
}
inline GLuint glGetUniformBlockIndex(GLuint program,
                                     const GLchar* uniformBlockName,
                                     const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glGetUniformBlockIndex, program,
                uniformBlockName);
}
inline GLint glGetUniformLocation(GLuint program, const GLchar* name,
                                  const sl& sourceLocation = sl::current()) {
  return callGL(sourceLocation, ::glGetUniformLocation, program, name);
}
inline void glLinkProgram(GLuint program,
                          const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glLinkProgram, program);
}
inline void glRenderbufferStorage(GLenum target, GLenum internalformat,
                                  GLsizei width, GLsizei height,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glRenderbufferStorage, target, internalformat, width,
         height);
}
inline void glShaderSource(GLuint shader, GLsizei count, const GLchar** string,
                           const GLint* length,
                           const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glShaderSource, shader, count, string, length);
}
inline void glTexImage2D(GLenum target, GLint level, GLint internalformat,
                         GLsizei width, GLsizei height, GLint border,
                         GLenum format, GLenum type, const void* data,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexImage2D, target, level, internalformat, width,
         height, border, format, type, data);
}
inline void glTexImage2DMultisample(GLenum target, GLsizei samples,
                                    GLenum internalformat, GLsizei width,
                                    GLsizei height,
                                    GLboolean fixedsamplelocations,
                                    const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexImage2DMultisample, target, samples,
         internalformat, width, height, fixedsamplelocations);
}
inline void glTexParameteri(GLenum target, GLenum pname, GLint param,
                            const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glTexParameteri, target, pname, param);
}
inline void glUniform1f(GLint location, GLfloat v0,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform1f, location, v0);
}
inline void glUniform1i(GLint location, GLint v0,
                        const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform1i, location, v0);
}
inline void glUniform3fv(GLint location, GLsizei count, const GLfloat* value,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniform3fv, location, count, value);
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
inline void glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex,
                                  GLuint uniformBlockBinding,
                                  const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUniformBlockBinding, program, uniformBlockIndex,
         uniformBlockBinding);
}
inline void glUseProgram(GLuint program,
                         const sl& sourceLocation = sl::current()) {
  callGL(sourceLocation, ::glUseProgram, program);
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
#endif
}  // namespace abcg

#endif