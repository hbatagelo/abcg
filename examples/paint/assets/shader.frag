#version 300 es

precision mediump float;

uniform vec4 u_color;
uniform bool u_apply;
uniform float u_time;
uniform vec3 u_random;
uniform float u_alpha;
uniform ivec2 u_viewport;

in vec2 position;
out vec4 outColor;

void main() { 
    if (!u_apply) {
        outColor = u_color;
    } else {
        vec2 norm = gl_FragCoord.xy/float(u_viewport);
        vec3 c = sqrt(u_time) * (norm.xyx + u_random);
        vec3 col = (1.0 + cos(c))/2.0;
        outColor = vec4(col, u_alpha);
    }
}
