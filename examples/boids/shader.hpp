#ifndef SHADER_HPP_
#define SHADER_HPP_

#include "abcgOpenGL.hpp"

struct Shader {
    Shader() {
        const auto assetsPath = abcg::Application::getAssetsPath();
        program = abcg::createOpenGLProgram({
            {.source = assetsPath + "shader.vert", .stage = abcg::ShaderStage::Vertex},
            {.source = assetsPath + "shader.frag", .stage = abcg::ShaderStage::Fragment}
        });
        positionALoc = abcg::glGetAttribLocation(program, "l_position");

        modelULoc = abcg::glGetUniformLocation(program, "u_model");
        viewULoc = abcg::glGetUniformLocation(program, "u_view");
        projULoc = abcg::glGetUniformLocation(program, "u_proj");

        stripULoc = abcg::glGetUniformLocation(program, "u_strip");
        AcolorULoc = abcg::glGetUniformLocation(program, "u_Acolor");
        DcolorULoc = abcg::glGetUniformLocation(program, "u_Dcolor");
        ScolorULoc = abcg::glGetUniformLocation(program, "u_Scolor");
    }

    GLuint program;
    GLint positionALoc;
    GLint modelULoc;
    GLint viewULoc;
    GLint projULoc;

    GLint stripULoc;
    GLint AcolorULoc;
    GLint DcolorULoc;
    GLint ScolorULoc;
};

#endif
