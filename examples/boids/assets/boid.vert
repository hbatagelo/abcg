#version 300 es

layout(location = 0) in vec3 l_Position;
layout(location = 1) in vec3 l_Normal;
layout(location = 2) in vec2 l_Tex;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

out vec3 io_WorldPosition;
out vec3 io_Normal;
out vec2 io_TexCoord;

void main() {
    io_WorldPosition = vec3(u_Model * vec4(l_Position, 1.0));
    io_Normal = mat3(transpose(inverse(u_Model))) * l_Normal;
    io_TexCoord = l_Tex;

    gl_Position = u_Proj * u_View * vec4(io_WorldPosition, 1.0);
}
