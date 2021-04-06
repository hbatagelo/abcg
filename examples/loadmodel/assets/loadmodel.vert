#version 410 core

layout(location = 0) in vec3 inPosition;

uniform float angle;

void main() {
  float sinAngle = sin(angle);
  float cosAngle = cos(angle);

  gl_Position = vec4( inPosition.x * cosAngle + inPosition.z * sinAngle,
                      inPosition.y,
                      inPosition.z * cosAngle - inPosition.x * sinAngle, 1.0);
}