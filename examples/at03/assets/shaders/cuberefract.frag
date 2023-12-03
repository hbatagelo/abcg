#version 300 es

precision mediump float;

in vec3 fragP;
in vec3 fragN;

uniform mat3 texMatrix;
uniform samplerCube cubeTex;

out vec4 outColor;

void main() {
  const float n1 = 1.0;  // Air/vacuum
  const float n2 = 1.52; // Glass

  vec3 V = normalize(-fragP);
  vec3 N = normalize(fragN);
  vec3 T = refract(-V, N, n1 / n2);

  outColor = texture(cubeTex, texMatrix * T);
}