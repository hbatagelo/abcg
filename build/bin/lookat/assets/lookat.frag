#version 410

in vec4 fragColor;
out vec4 outColor;

void main() {
  if (gl_FrontFacing) {
    outColor = fragColor;
  } else {
    outColor = fragColor * 0.5;
  }
}