#version 330 core

layout(location = 0) out vec4 f_color;

in vec3 v_normal;
in vec2 v_texCoord;
in vec3 v_fragmentPosition;

struct Material {
    sampler2D texture_diffuse0;
    sampler2D texture_diffuse1;
    sampler2D texture_specular0;
    sampler2D texture_specular1;
    float shininess;
};
uniform Material u_material;

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight u_directionalLight;

struct PointLight {
    vec3 position;
    
    float K_c;
    float K_l;
    float K_q;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 1
uniform PointLight u_pointLight[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;

    float innerCutOff;
    float outerCutOff;

    float K_c;
    float K_l;
    float K_q;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight u_spotLight;

uniform mat3 u_normalMatrix;
uniform vec3 u_viewPosition;

vec3 directionalLightContribution(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 pointLightContribution(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 spotLightContribution(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {

    vec3 fragmentColor = vec3(0.0f);

    vec3 normal = normalize(v_normal);
    vec3 viewDir = normalize(u_viewPosition - v_fragmentPosition);

    fragmentColor += directionalLightContribution(u_directionalLight, normal, viewDir);

    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        fragmentColor += pointLightContribution(u_pointLight[i], normal, v_fragmentPosition, viewDir);
    }

    //fragmentColor += spotLightContribution(u_spotLight, normal, v_fragmentPosition, viewDir);

    f_color = vec4(fragmentColor, 1.0f);

}

vec3 directionalLightContribution(DirectionalLight light, vec3 normal, vec3 viewDir) {

    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diffuseShading = max(dot(normal, lightDir), 0.0f);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularShading = pow(max(dot(viewDir, reflectDir), 0.0f), u_material.shininess);

    vec3 diffuseTex = vec3(texture(u_material.texture_diffuse0, v_texCoord));
    vec3 specularTex = vec3(texture(u_material.texture_specular0, v_texCoord));

    vec3 ambient = light.ambient * diffuseTex;
    vec3 diffuse = light.diffuse * diffuseShading * diffuseTex;
    vec3 specular = light.specular * specularShading * specularTex;

    return (ambient + diffuse + specular);
}

vec3 pointLightContribution(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diffuseShading = max(dot(normal, lightDir), 0.0f);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularShading = pow(max(dot(viewDir, reflectDir), 0.0f), u_material.shininess);

    // Attenuation
    float distanceLightFragment = length(light.position - fragPos);
    float attenuation = 1.0f / (light.K_c + light.K_l * distanceLightFragment + light.K_q * distanceLightFragment * distanceLightFragment);    

    vec3 diffuseTex = vec3(texture(u_material.texture_diffuse0, v_texCoord));
    vec3 specularTex = vec3(texture(u_material.texture_specular0, v_texCoord));

    vec3 ambient = light.ambient * diffuseTex;
    vec3 diffuse = light.diffuse * diffuseShading * diffuseTex;
    vec3 specular = light.specular * specularShading * specularTex;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 spotLightContribution(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diffuseShading = max(dot(normal, lightDir), 0.0f);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularShading = pow(max(dot(viewDir, reflectDir), 0.0f), u_material.shininess);

    // Attenuation
    float distanceLightFragment = length(light.position - fragPos);
    float attenuation = 1.0f / (light.K_c + light.K_l * distanceLightFragment + light.K_q * distanceLightFragment * distanceLightFragment);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

    vec3 diffuseTex = vec3(texture(u_material.texture_diffuse0, v_texCoord));
    vec3 specularTex = vec3(texture(u_material.texture_specular0, v_texCoord));
    
    vec3 ambient = light.ambient * diffuseTex;
    vec3 diffuse = light.diffuse * diffuseShading * diffuseTex;
    vec3 specular = light.specular * specularShading * specularTex;

    diffuse *= intensity;
    specular *= intensity;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}