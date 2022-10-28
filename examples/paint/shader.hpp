#ifndef SHADER_HPP_
#define SHADER_HPP_

#include "abcgOpenGL.hpp"

struct Shader {
    GLuint program;
    GLint positionALoc;
    GLint modelULoc;
    GLint colorlULoc;
    GLint applylULoc;
    GLint timelULoc;
    GLint randomlULoc;
    GLint alphalULoc;
};

#endif
