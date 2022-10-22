#version 330 core

layout(location = 0) out vec4 f_color;

in vec3 v_normal;
in vec2 v_texCoord;
in vec3 v_fragmentPosition;

in vec3 v_lightDirection;
in vec3 v_lightPosition0;

in vec3 v_viewPosition;

struct Material
{
    sampler2D texture_diffuse;
    sampler2D texture_normal;
};
uniform Material u_material;

struct DirectionalLight
{
    vec3 direction;
    bool isActive;
    vec3 color;
};
uniform DirectionalLight u_directionalLight;

struct PointLight
{
    vec3 position;
    bool isActive;
    vec3 color;
};

#define NUM_POINT_LIGHTS 1
uniform PointLight u_pointLight[NUM_POINT_LIGHTS];

// clang-format off
vec3 sampleOffsetDirections[20] = vec3[] (
    vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
    vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
    vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
    vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);
// clang-format on

vec3 directionalLightContribution(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 pointLightContribution(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void computeShading(vec3 light_ambient, vec3 light_diffuse, vec3 light_specular, vec3 lightDir, vec3 viewDir,
                    vec3 normal, out vec3 ambient, out vec3 diffuse, out vec3 specular);

float computeAttenuation(vec3 lightPos, vec3 fragPos, float K_q);

void main()
{
    vec3 fragmentColor = vec3(0.0f);

    vec3 normal = texture(u_material.texture_normal, v_texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 viewDir = normalize(v_viewPosition - v_fragmentPosition);

    fragmentColor += directionalLightContribution(u_directionalLight, normal, viewDir);

    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
        fragmentColor += pointLightContribution(u_pointLight[i], normal, v_fragmentPosition, viewDir);
    }
    
    f_color = vec4(fragmentColor, 1.0f);
}

vec3 directionalLightContribution(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    // Only compute if light source is active
    if (!light.isActive)
        return vec3(0.0f);

    vec3 lightDir = normalize(-v_lightDirection);

    vec3 diffuseColor = light.color;
    vec3 specularColor = light.color * 0.5f;
    vec3 ambientColor = light.color * 0.002f;

    vec3 ambient, diffuse, specular;
    computeShading(ambientColor, diffuseColor, specularColor, lightDir, viewDir, normal, ambient, diffuse, specular);

    return ambient + diffuse + specular;
}

vec3 pointLightContribution(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Only compute if light source is active
    if (!light.isActive)
        return vec3(0.0f);

    vec3 lightDir = normalize(v_lightPosition0 - fragPos);

    vec3 diffuseColor = light.color;
    vec3 specularColor = light.color * 0.5f;
    vec3 ambientColor = light.color * 0.002f;

    vec3 ambient, diffuse, specular;
    computeShading(ambientColor, diffuseColor, specularColor, lightDir, viewDir, normal, ambient, diffuse, specular);

    float attenuation = computeAttenuation(v_lightPosition0, fragPos, 0.032f);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void computeShading(vec3 light_ambient, vec3 light_diffuse, vec3 light_specular, vec3 lightDir, vec3 viewDir,
                    vec3 normal, out vec3 ambient, out vec3 diffuse, out vec3 specular)
{
    // Diffuse shading
    float diffuseShading = max(dot(normal, lightDir), 0.0f);

    // Specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specularShading = pow(max(dot(normal, halfwayDir), 0.0f), 100.0f);

    vec4 diffuseTex = texture(u_material.texture_diffuse, v_texCoord);
    // vec4 specularTex = texture(u_material.texture_specular, v_texCoord);

    ambient = light_ambient * vec3(diffuseTex);
    diffuse = light_diffuse * diffuseShading * vec3(diffuseTex);
    specular = light_specular * specularShading * vec3(1.0f);//vec3(specularTex);
}

float computeAttenuation(vec3 lightPos, vec3 fragPos, float K_q)
{
    float distanceLightFragment = length(lightPos - fragPos);

    return 1.0f / (K_q * distanceLightFragment * distanceLightFragment);
}
