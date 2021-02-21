#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;

out vec3 v_normal;
out vec2 v_texCoord;

out vec3 v_fragmentPosition;
out vec3 v_fragmentPositionTangent;
out vec4 v_fragmentPositionDirectionalLightSpace;

out vec3 v_viewPositionTangent;

struct DirectionalLight {
    vec3 direction;
    bool isActive;
    vec3 color;
};
uniform DirectionalLight u_directionalLight;
out vec3 v_lightDirectionTangent;

struct PointLight {
    vec3 position;
    bool isActive;
    vec3 color;
};
#define NUM_POINT_LIGHTS 1
uniform PointLight u_pointLight[NUM_POINT_LIGHTS];
out vec3 v_lightPositionTangent0;
//out vec3 v_lightPositionTangent1;
//out vec3 v_lightPositionTangent2;
//out vec3 v_lightPositionTangent3;

uniform vec3 u_viewPosition;

uniform mat4 u_modelViewProjMatrix;
uniform mat4 u_modelMatrix;

uniform mat4 u_directionalLightViewProjMatrix;

void main() {
    gl_Position = u_modelViewProjMatrix * vec4(a_position, 1.0f);

    vec3 T = normalize(vec3(u_modelMatrix * vec4(a_tangent, 0.0f)));
    vec3 B = normalize(vec3(u_modelMatrix * vec4(a_bitangent, 0.0f)));
    vec3 N = normalize(vec3(u_modelMatrix * vec4(a_normal, 0.0f)));
    mat3 TBN_transposed = transpose(mat3(T, B, N));

    v_lightDirectionTangent = TBN_transposed * u_directionalLight.direction;
    v_lightPositionTangent0 = TBN_transposed * u_pointLight[0].position;
    //v_lightPositionTangent1 = vec3(0.0f);
    //v_lightPositionTangent2 = vec3(0.0f);
    //v_lightPositionTangent3 = vec3(0.0f);

    v_fragmentPositionTangent = TBN_transposed * vec3(u_modelMatrix * vec4(a_position, 1.0f));

    v_fragmentPosition = vec3(u_modelMatrix * vec4(a_position, 1.0f));
    v_fragmentPositionDirectionalLightSpace = u_directionalLightViewProjMatrix * vec4(v_fragmentPosition, 1.0f);

    v_viewPositionTangent = TBN_transposed * u_viewPosition;

    v_normal = a_normal;
    v_texCoord = a_texCoord;
}
