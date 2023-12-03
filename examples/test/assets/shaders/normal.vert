#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

out vec4 fragColor;

void main() {
  mat4 MVP = projMatrix * viewMatrix * modelMatrix;

  gl_Position = MVP * vec4(inPosition, 1.0);

  vec3 N = inNormal;  // Object space
  // vec3 N = normalMatrix * inNormal; // Eye space

  // Convert from [-1,1] to [0,1]
  fragColor = vec4((N + 1.0) / 2.0, 1.0);
}