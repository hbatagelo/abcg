#version 300 es

precision mediump float;

in vec3 fragN;
in vec3 fragL;
in vec3 fragV;
in vec2 fragTexCoord;
in vec3 fragPObj;
in vec3 fragNObj;

// Light properties
uniform vec4 Ia, Id, Is;

// Material properties
uniform vec4 Ka, Kd, Ks;
uniform float shininess;

// Diffuse texture sampler
uniform sampler2D diffuseTex;

// Mapping mode
// 0: triplanar; 1: cylindrical; 2: spherical; 3: from mesh
uniform int mappingMode;

out vec4 outColor;

// Blinn-Phong reflection model
vec4 BlinnPhong(vec3 N, vec3 L, vec3 V, vec2 texCoord) {
  N = normalize(N);
  L = normalize(L);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    V = normalize(V);
    vec3 H = normalize(L + V);
    float angle = max(dot(H, N), 0.0);
    specular = pow(angle, shininess);
  }

  vec4 map_Kd = texture(diffuseTex, texCoord);
  vec4 map_Ka = map_Kd;

  vec4 diffuseColor = map_Kd * Kd * Id * lambertian;
  vec4 specularColor = Ks * Is * specular;
  vec4 ambientColor = map_Ka * Ka * Ia;

  return ambientColor + diffuseColor + specularColor;
}

// Planar mapping
vec2 PlanarMappingX(vec3 P) { return vec2(1.0 - P.z, P.y); }
vec2 PlanarMappingY(vec3 P) { return vec2(P.x, 1.0 - P.z); }
vec2 PlanarMappingZ(vec3 P) { return P.xy; }

#define PI 3.14159265358979323846

// Cylindrical mapping
vec2 CylindricalMapping(vec3 P) {
  float longitude = atan(P.x, P.z);
  float height = P.y;

  float u = longitude / (2.0 * PI) + 0.5;  // From [-pi, pi] to [0, 1]
  float v = height - 0.5;                  // Base at y = -0.5

  return vec2(u, v);
}

// Spherical mapping
vec2 SphericalMapping(vec3 P) {
  float longitude = atan(P.x, P.z);
  float latitude = asin(P.y / length(P));

  float u = longitude / (2.0 * PI) + 0.5;  // From [-pi, pi] to [0, 1]
  float v = latitude / PI + 0.5;           // From [-pi/2, pi/2] to [0, 1]

  return vec2(u, v);
}

void main() {
  vec4 color;

  if (mappingMode == 0) {
    // Triplanar mapping

    // An offset to center the texture around the origin
    vec3 offset = vec3(-0.5, -0.5, -0.5);    

    // Sample with x planar mapping
    vec2 texCoord1 = PlanarMappingX(fragPObj + offset);
    vec4 color1 = BlinnPhong(fragN, fragL, fragV, texCoord1);

    // Sample with y planar mapping
    vec2 texCoord2 = PlanarMappingY(fragPObj + offset);
    vec4 color2 = BlinnPhong(fragN, fragL, fragV, texCoord2);

    // Sample with z planar mapping
    vec2 texCoord3 = PlanarMappingZ(fragPObj + offset);
    vec4 color3 = BlinnPhong(fragN, fragL, fragV, texCoord3);

    // Compute average based on normal
    vec3 weight = abs(normalize(fragNObj));
    color = color1 * weight.x + color2 * weight.y + color3 * weight.z;
  } else {
    vec2 texCoord;
    if (mappingMode == 1) {
      // Cylindrical mapping
      texCoord = CylindricalMapping(fragPObj);
    } else if (mappingMode == 2) {
      // Spherical mapping
      texCoord = SphericalMapping(fragPObj);
    } else if (mappingMode == 3) {
      // From mesh
      texCoord = fragTexCoord;
    }
    color = BlinnPhong(fragN, fragL, fragV, texCoord);
  }

  if (gl_FrontFacing) {
    outColor = color;
  } else {
    float i = (color.r + color.g + color.b) / 3.0;
    outColor = vec4(i, 0, 0, 1.0);
  }
}
