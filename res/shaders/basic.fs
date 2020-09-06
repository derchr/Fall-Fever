#version 330 core

layout(location = 0) out vec4 f_color;

struct Material {
    sampler2D u_texture_diffuse0;
    sampler2D u_texture_diffuse1;
    sampler2D u_texture_specular0;
    sampler2D u_texture_specular1;
};

in vec3 v_normal;
in vec4 v_color;
in vec2 v_texCoord;

in vec3 v_fragmentPosition;

uniform vec3 u_objectColor;

uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;

uniform mat3 u_normalMatrix;

uniform vec3 u_viewPosition;

uniform Material material;


void main() {
    float ambientStrength = 0.1f;
    float specularStrength = 0.5;

    vec3 ambient = ambientStrength * u_lightColor;

    // Diffuse lighting
    vec3 normal = normalize(u_normalMatrix * v_normal);
    vec3 lightDir = normalize(u_lightPosition - v_fragmentPosition);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * u_lightColor;

    // Specular lighting
    vec3 viewDir = normalize(u_viewPosition - v_fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * u_lightColor;  

    //f_color = v_color;
    //vec4 texColor1 = texture(material.u_texture_diffuse0, v_texCoord);
    //vec4 texColor2 = texture(material.u_texture_diffuse1, v_texCoord);
    //f_color = texColor1;


    f_color = vec4((ambient + diffuse + specular) * u_objectColor, 1.0f);
}
