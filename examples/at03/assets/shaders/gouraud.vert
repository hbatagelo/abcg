#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

// Light properties
uniform vec4 lightDirWorldSpace;
uniform vec4 Ia, Id, Is;

// Material properties
uniform vec4 Ka, Kd, Ks;
uniform float shininess;

out vec4 fragColor;

vec4 Phong(vec3 N, vec3 L, vec3 V) {
  N = normalize(N);
  L = normalize(L);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    // vec3 R = normalize(2.0 * dot(N, L) * N - L);
    vec3 R = reflect(-L, N);
    V = normalize(V);
    float angle = max(dot(R, V), 0.0);
    specular = pow(angle, shininess);
  }

  vec4 diffuseColor = Kd * Id * lambertian;
  vec4 specularColor = Ks * Is * specular;
  vec4 ambientColor = Ka * Ia;

  return ambientColor + diffuseColor + specularColor;
}

void main() {
  vec3 P = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  vec3 N = normalMatrix * inNormal;
  vec3 L = -(viewMatrix * lightDirWorldSpace).xyz;
  vec3 V = -P;

  fragColor = Phong(N, L, V);

  gl_Position = projMatrix * vec4(P, 1.0);
}
