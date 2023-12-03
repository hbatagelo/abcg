#version 300 es

precision mediump float;

in vec4 fragColor;
out vec4 outColor;

void main() {
  if (gl_FrontFacing) {
    outColor = fragColor;
  } else {
    float i = (fragColor.r + fragColor.g + fragColor.b) / 3.0;
    outColor = vec4(i, 0, 0, 1.0);
  }
}
