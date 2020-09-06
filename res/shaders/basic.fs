#version 330 core

layout(location = 0) out vec4 f_color;

struct Material {
    sampler2D u_texture_diffuse0;
    sampler2D u_texture_diffuse1;
    sampler2D u_texture_specular0;
    sampler2D u_texture_specular1;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

in vec3 v_normal;
in vec4 v_color;
in vec2 v_texCoord;

in vec3 v_fragmentPosition;

uniform mat3 u_normalMatrix;

uniform vec3 u_viewPosition;



void main() {
    float specularStrength = 0.5;

    vec3 ambient = light.ambient * material.ambient;

    // Diffuse lighting
    vec3 normal = normalize(u_normalMatrix * v_normal);
    vec3 lightDir = normalize(light.position - v_fragmentPosition);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Specular lighting
    vec3 viewDir = normalize(u_viewPosition - v_fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  

    //f_color = v_color;
    //vec4 texColor1 = texture(material.u_texture_diffuse0, v_texCoord);
    //vec4 texColor2 = texture(material.u_texture_diffuse1, v_texCoord);
    //f_color = texColor1;


    f_color = vec4((ambient + diffuse + specular), 1.0f);
}
