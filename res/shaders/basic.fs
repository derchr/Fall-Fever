#version 330 core

layout(location = 0) out vec4 f_color;

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

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light u_light;

in vec3 v_normal;
in vec2 v_texCoord;
in vec3 v_fragmentPosition;

uniform mat3 u_normalMatrix;
uniform vec3 u_viewPosition;

void main() {

    // Diffuse lighting
    vec3 diffuseColor0 = vec3(texture(u_material.texture_diffuse0, v_texCoord));
    vec3 normal = normalize(u_normalMatrix * v_normal);
    vec3 lightDir = normalize(u_light.position - v_fragmentPosition);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = u_light.diffuse * diff * diffuseColor0;

    // Ambient lighting 
    vec3 ambient = u_light.ambient * diffuseColor0;

    // Specular lighting
    vec3 viewDir = normalize(u_viewPosition - v_fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
    vec3 specular = u_light.specular * spec * vec3(texture(u_material.texture_specular0, v_texCoord));

    //f_color = v_color;
    //vec4 texColor1 = texture(u_material.u_texture_diffuse0, v_texCoord);
    //vec4 texColor2 = texture(u_material.u_texture_diffuse1, v_texCoord);
    //f_color = texColor1;

    f_color = vec4((ambient + diffuse + specular), 1.0f);
}
