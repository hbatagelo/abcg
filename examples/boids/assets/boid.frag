#version 300 es

precision mediump float;

in vec3 position;
out vec4 outColor;

void main() {
    outColor = vec4((position+1.0)/2.0, 1.0);
}
