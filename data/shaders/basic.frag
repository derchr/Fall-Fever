#version 330 core

layout(location = 0) out vec4 f_color;

in vec3 v_normal;
in vec2 v_texCoord;
in vec3 v_fragmentPosition;
in vec3 v_fragmentPositionTangent;
in vec4 v_fragmentPositionDirectionalLightSpace;

in vec3 v_lightDirectionTangent;
in vec3 v_lightPositionTangent0;
//in vec3 v_lightPositionTangent1;
//in vec3 v_lightPositionTangent2;
//in vec3 v_lightPositionTangent3;

in vec3 v_viewPositionTangent;

struct Material {
    sampler2D texture_diffuse0;
    sampler2D texture_diffuse1;
    sampler2D texture_specular0;
    sampler2D texture_normal0;
    sampler2D texture_height0;
    sampler2D texture_gloss0;
    float shininess;
};
uniform Material u_material;

struct DirectionalLight {
    vec3 direction;
    bool isActive;
    vec3 color;
};
uniform DirectionalLight u_directionalLight;

struct PointLight {
    vec3 position;
    bool isActive;
    vec3 color;
};
#define NUM_POINT_LIGHTS 1
uniform PointLight u_pointLight[NUM_POINT_LIGHTS];

/*struct SpotLight {
    bool isActive;
    vec3 position;
    vec3 direction;

    float innerCutOff;
    float outerCutOff;

    float K_q;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight u_spotLight;*/

uniform mat3 u_normalMatrix;

// uniform sampler2D u_texture_directionalShadowMap;
// uniform samplerCube u_texture_pointShadowMap0;
//uniform samplerCube u_texture_pointShadowMap1;
//uniform samplerCube u_texture_pointShadowMap2;
//uniform samplerCube u_texture_pointShadowMap3;

vec3 sampleOffsetDirections[20] = vec3[] (
    vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
    vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
    vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
    vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

// uniform bool b_drawShadows;

// uniform float pointShadowDepthMapFarPlane;

vec3 directionalLightContribution(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 pointLightContribution(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 spotLightContribution(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void computeShading(
    vec3 light_ambient, vec3 light_diffuse, vec3 light_specular,
    vec3 lightDir, vec3 viewDir, vec3 normal,
    out vec3 ambient, out vec3 diffuse, out vec3 specular
);

float computeAttenuation(vec3 lightPos, vec3 fragPos, float K_q);

// float computeDirectionalShadows(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
// float computePointShadows(vec3 fragPos, vec3 lightPos);

void main() {

    vec3 fragmentColor = vec3(0.0f);

    vec3 normal = texture(u_material.texture_normal0, v_texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 viewDir = normalize(v_viewPositionTangent - v_fragmentPositionTangent);

    fragmentColor += directionalLightContribution(u_directionalLight, normal, viewDir);

    for(int i = 0; i < NUM_POINT_LIGHTS; i++) {
        fragmentColor += pointLightContribution(u_pointLight[i], normal, v_fragmentPositionTangent, viewDir);
    }

    // There are currently no spotlights
    //fragmentColor += spotLightContribution(u_spotLight, normal, v_fragmentPositionTangent, viewDir);

    f_color = vec4(fragmentColor, 1.0f);
    f_color = vec4(0.95f, 0.16f, 0.33f, 1.0f);
}

vec3 directionalLightContribution(DirectionalLight light, vec3 normal, vec3 viewDir) {

    // Only compute if light source is active
    if(!light.isActive)
        return vec3(0.0f);

    //vec3 lightDir = normalize(-light.direction);
    vec3 lightDir = normalize(-v_lightDirectionTangent);

    vec3 diffuseColor = light.color;
    vec3 specularColor = light.color * 0.5f;
    vec3 ambientColor = light.color * 0.002f;

    vec3 ambient, diffuse, specular;
    computeShading(ambientColor, diffuseColor, specularColor, lightDir, viewDir, normal, ambient, diffuse, specular);

    float shadows = 0.0f;
    // if(b_drawShadows)
    //     shadows = computeDirectionalShadows(v_fragmentPositionDirectionalLightSpace, normal, lightDir);

    return (ambient + (1.0f - shadows) * (diffuse + specular));
}

vec3 pointLightContribution(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

    // Only compute if light source is active
    if(!light.isActive)
        return vec3(0.0f);

    vec3 lightDir = normalize(v_lightPositionTangent0 - fragPos);

    vec3 diffuseColor = light.color;
    vec3 specularColor = light.color * 0.5f;
    vec3 ambientColor = light.color * 0.002f;

    vec3 ambient, diffuse, specular;
    computeShading(ambientColor, diffuseColor, specularColor, lightDir, viewDir, normal, ambient, diffuse, specular);

    float attenuation = computeAttenuation(v_lightPositionTangent0, fragPos, 0.032f);
    //ambient *= attenuation;
    //diffuse *= attenuation;
    //specular *= attenuation;

    float shadows = 0.0f;
    // if(b_drawShadows)
    //     shadows = computePointShadows(v_fragmentPosition, light.position);

    return (ambient + (1.0f - shadows) * (diffuse + specular));
}

/*vec3 spotLightContribution(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

    // Only compute if light source is active
    if(!light.isActive)
        return vec3(0.0f);

    vec3 lightDir = normalize(light.position - fragPos);

    vec3 ambient, diffuse, specular;
    computeShading(light.ambient, light.diffuse, light.specular, lightDir, viewDir, normal, ambient, diffuse, specular);

    float attenuation = computeAttenuation(light.position, fragPos, light.K_q);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

    diffuse *= intensity;
    specular *= intensity;

    return (ambient + diffuse + specular);
}*/

void computeShading(
    vec3 light_ambient, vec3 light_diffuse, vec3 light_specular,
    vec3 lightDir, vec3 viewDir, vec3 normal,
    out vec3 ambient, out vec3 diffuse, out vec3 specular) {
            
    // Diffuse shading
    float diffuseShading = max(dot(normal, lightDir), 0.0f);

    // Specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specularShading = pow(max(dot(normal, halfwayDir), 0.0f), u_material.shininess);
    
    vec4 diffuseTex = texture(u_material.texture_diffuse0, v_texCoord);
    vec4 specularTex = texture(u_material.texture_specular0, v_texCoord);

    ambient = light_ambient * vec3(diffuseTex);
    diffuse = light_diffuse * diffuseShading * vec3(diffuseTex);
    specular = light_specular * specularShading * vec3(specularTex);

}

float computeAttenuation(vec3 lightPos, vec3 fragPos, float K_q) {

    float distanceLightFragment = length(lightPos - fragPos);

    return 1.0f / (K_q * distanceLightFragment * distanceLightFragment);

}

// float computeDirectionalShadows(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {

//     // Perspective divide
//     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

//     // Transform from [-1,1] to [0,1]
//     projCoords *= 0.5f;
//     projCoords += 0.5f;

//     if(projCoords.z > 1.0f) return 0.0f;

//     float closestDepth = texture(u_texture_directionalShadowMap, projCoords.xy).r;
//     float currentDepth = projCoords.z;

//     float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
//     bias *= 0.25f;

//     float shadow = 0.0;
//     vec2 texelSize = 1.0 / textureSize(u_texture_directionalShadowMap, 0);
//     for(int x = -1; x <= 1; x++) {
//         for(int y = -1; y <= 1; y++) {
//             float pcfDepth = texture(u_texture_directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
//             shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
//         }
//     }
//     shadow /= 9.0f;

//     return shadow;
// }

// float computePointShadows(vec3 fragPos, vec3 lightPos) {

//     // get vector between fragment position and light position
//     vec3 fragToLight = fragPos - lightPos;

//     // now get current linear depth as the length between the fragment and light position
//     float currentDepth = length(fragToLight);

//     float shadow = 0.0;
//     float bias = 0.05;
//     int samples = 20;
//     float viewDistance = length(v_viewPositionTangent - fragPos);
//     float diskRadius = 0.05;

//     for(int i = 0; i < samples; ++i) {
//         float closestDepth = texture(u_texture_pointShadowMap0, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
//         closestDepth *= pointShadowDepthMapFarPlane;
//         if(currentDepth - bias > closestDepth)
//             shadow += 1.0;
//     }

//     shadow /= float(samples);
//     return shadow;
// }
