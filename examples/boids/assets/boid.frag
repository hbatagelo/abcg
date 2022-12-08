#version 300 es

precision mediump float;

struct Material {
    sampler2D diffuseTexture;
    float shininess;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight {
    vec3 direction;
    float strength;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NUM_POINT_LIGHTS 15

uniform vec3 u_CamPosition;
uniform Material u_Material;
uniform DirLight u_DirLight;
uniform PointLight u_PointLights[NUM_POINT_LIGHTS];

in vec3 io_WorldPosition;
in vec3 io_Normal;
in vec2 io_TexCoord;

out vec4 outColor;

vec3 CalcLight(vec3 lightDir, vec3 a, vec3 d, vec3 s, vec3 normal, vec3 viewDir) {
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), u_Material.shininess);

    vec3 diffuseTexVal = vec3(texture(u_Material.diffuseTexture, io_TexCoord));

    // combine results
    vec3 ambient = a * u_Material.ambient * diffuseTexVal;
    vec3 diffuse = d * u_Material.diffuse * diffuseTexVal * diff;
    vec3 specular = s * u_Material.specular * spec;
    return (ambient + diffuse + specular);
}

vec3 DirectLightContribution(vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-u_DirLight.direction);
    return u_DirLight.strength*CalcLight(lightDir, u_DirLight.ambient, u_DirLight.diffuse, u_DirLight.specular, normal, viewDir);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - io_WorldPosition);

    // attenuation
    float d = length(light.position - io_WorldPosition);
    float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * (d * d));

    return attenuation*CalcLight(lightDir, light.ambient, light.diffuse, light.specular, normal, viewDir);;
}

void main() {
    vec3 norm = normalize(io_Normal);
    vec3 viewDir = normalize(u_CamPosition - io_WorldPosition);
    vec3 color = DirectLightContribution(norm, viewDir);

    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
        color += CalcPointLight(u_PointLights[i], norm, viewDir);
    }

    outColor = vec4(color, 1.0);
}
