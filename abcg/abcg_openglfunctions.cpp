/**
 * @file abcg_openglfunctions.cpp
 * @brief Definition of OpenGL-related error checking functions.
 *
 * This project is released under the MIT License.
 */

#include "abcg_openglfunctions.hpp"

#include "abcg_exception.hpp"
#include "abcg_external.hpp"

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
/**
 * @brief Checks OpenGL error status and throws on error with a log message.
 *
 * @param sourceLocation Information about the source code, used for logging.
 * @param prefix String view to be prefixed to the error message.
 *
 * @throw abcg::Exception with a log message.
 */
void abcg::checkGLError(
    const std::experimental::source_location &sourceLocation,
    std::string_view prefix) {
  if (auto status{glGetError()}; status != GL_NO_ERROR) {
    throw abcg::Exception{
        abcg::Exception::OpenGL(prefix, status, sourceLocation)};
  }
}
#endif