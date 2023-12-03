#version 300 es

precision mediump float;

in vec3 fragTexCoord;

out vec4 outColor;

uniform samplerCube skyTex;

void main() { outColor = texture(skyTex, fragTexCoord); }