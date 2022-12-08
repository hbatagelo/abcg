#version 300 es

precision mediump float;

uniform int u_strip;
uniform vec3 u_Acolor;
uniform vec3 u_Dcolor;
uniform vec3 u_Scolor;

in vec3 position;
out vec4 outColor;

void main() {
    if (u_strip == 0) {
        outColor = vec4(u_Acolor, 1.0);
        return;
    }

    vec3 color;
    if (position.y >= -1.0 && position.y < -0.333) {
        color = u_Scolor;
    } else if (position.y >= -0.333 && position.y < 0.333) {
        color = u_Dcolor;
    } else if (position.y >= 0.333 && position.y <= 1.0) {
        color = u_Acolor;
    }
    outColor = vec4(color, 1.0);
}
