#version 300 es

precision mediump float;

in vec3 position;
out vec4 outColor;

void main() {
    // outColor = vec4(position+0.5, 1.0);
    outColor = vec4(1.0);
}
