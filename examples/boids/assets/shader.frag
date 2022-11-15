#version 300 es

precision mediump float;

in vec3 position;
out vec4 outColor;

void main() {
    // vec3 c = position + 0.5;
    // outColor = vec4(c , 1);
    outColor = vec4(1, 1, 1, 1);
}
