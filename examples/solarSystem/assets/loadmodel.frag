#version 410

out vec4 outColor;

void main() {
  float i = 0.5 - gl_FragCoord.z;

  outColor = vec4(1, 1, i, 1);
}