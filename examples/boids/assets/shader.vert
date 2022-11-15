#version 300 es

layout(location = 0) in vec3 l_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec3 position;

void main() {
    position = l_position;
    gl_Position = u_proj * u_view * u_model * vec4(l_position, 1);
}
