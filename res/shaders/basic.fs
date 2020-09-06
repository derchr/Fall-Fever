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

struct Light {
    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float K_c;
    float K_l;
    float K_q;
};
uniform Light u_light;

uniform mat3 u_normalMatrix;
uniform vec3 u_viewPosition;

void main() {

    vec3 vecLightToFragment = u_light.position - v_fragmentPosition;
    vec3 lightDir = normalize(vecLightToFragment);
    //vec3 lightDir = normalize(-u_light.direction);

    // Diffuse lighting
    vec3 diffuseColor0 = vec3(texture(u_material.texture_diffuse0, v_texCoord));
    vec3 normal = normalize(u_normalMatrix * v_normal);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = u_light.diffuse * diff * diffuseColor0;

    // Ambient lighting 
    vec3 ambient = u_light.ambient * diffuseColor0;

    // Specular lighting
    vec3 viewDir = normalize(u_viewPosition - v_fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), u_material.shininess);
    vec3 specular = u_light.specular * spec * vec3(texture(u_material.texture_specular0, v_texCoord));

    float distanceLightFragment = length(vecLightToFragment);
    float attenuation = 1.0f / (u_light.K_c + u_light.K_l * distanceLightFragment + u_light.K_q * distanceLightFragment * distanceLightFragment);    

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float theta = dot(lightDir, normalize(-u_light.direction));
    float epsilon = u_light.innerCutOff - u_light.outerCutOff;
    float intensity = clamp((theta - u_light.outerCutOff) / epsilon, 0.0f, 1.0f);

    diffuse *= intensity;
    specular *= intensity;

    f_color = vec4(ambient + diffuse + specular, 1.0f);

}
