#version 300 es

precision mediump float;

in vec3 fragP;
in vec3 fragN;

uniform mat3 texMatrix;
uniform samplerCube cubeTex;

out vec4 outColor;

void main() {
  vec3 V = normalize(-fragP);
  vec3 N = normalize(fragN);
  vec3 R = reflect(-V, N);

  outColor = texture(cubeTex, texMatrix * R);
}