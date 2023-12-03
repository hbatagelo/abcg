#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inTangent;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec4 lightDirWorldSpace;

out vec2 fragTexCoord;
out vec3 fragPObj;
out vec3 fragTObj;
out vec3 fragBObj;
out vec3 fragNObj;
out vec3 fragLEye;
out vec3 fragVEye;

void main() {
  vec3 PEye = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  vec3 LEye = -(viewMatrix * lightDirWorldSpace).xyz;

  fragTexCoord = inTexCoord;

  fragPObj = inPosition;
  fragTObj = inTangent.xyz;
  fragBObj = inTangent.w * cross(inNormal, inTangent.xyz);
  fragNObj = inNormal;

  fragLEye = LEye;
  fragVEye = -PEye;

  gl_Position = projMatrix * vec4(PEye, 1.0);
}