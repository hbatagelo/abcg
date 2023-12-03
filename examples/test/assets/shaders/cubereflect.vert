#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

out vec3 fragP;
out vec3 fragN;

void main() {
  fragP = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  fragN = normalMatrix * inNormal;

  gl_Position = projMatrix * vec4(fragP, 1.0);
}