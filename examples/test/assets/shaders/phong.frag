#version 300 es

precision mediump float;

in vec3 fragN;
in vec3 fragL;
in vec3 fragV;

// Light properties
uniform vec4 Ia, Id, Is;

// Material properties
uniform vec4 Ka, Kd, Ks;
uniform float shininess;

out vec4 outColor;

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
  vec4 color = Phong(fragN, fragL, fragV);

  if (gl_FrontFacing) {
    outColor = color;
  } else {
    float i = (color.r + color.g + color.b) / 3.0;
    outColor = vec4(i, 0, 0, 1.0);
  }
}
