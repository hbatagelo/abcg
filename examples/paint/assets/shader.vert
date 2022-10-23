#version 300 es

layout(location = 0) in vec2 l_position;

uniform mat4 u_model;

out vec2 position;

void main() {
    position = l_position;
    gl_Position = u_model * vec4(l_position, 0, 1);
}
