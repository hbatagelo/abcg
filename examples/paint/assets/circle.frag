#version 300 es

precision mediump float;

uniform vec4 u_color;

in vec2 position;
out vec4 outColor;

void main() {
    vec2 pos = 2.0 * position;
    float dist = 0.4 - length(pos);
    if (dist > 0.0) {
        dist = 1.0;
    }
    outColor = dist * u_color;
}
