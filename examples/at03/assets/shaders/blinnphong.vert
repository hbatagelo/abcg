#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

uniform vec4 lightDirWorldSpace;

out vec3 fragV;
out vec3 fragL;
out vec3 fragN;

void main() {
  vec3 P = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  vec3 N = normalMatrix * inNormal;
  vec3 L = -(viewMatrix * lightDirWorldSpace).xyz;

  fragL = L;
  fragV = -P;
  fragN = N;

  gl_Position = projMatrix * vec4(P, 1.0);
}
