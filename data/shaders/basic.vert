#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_tangent;

out vec3 v_normal;
out vec2 v_texCoord;

out vec3 v_fragmentPosition;
out vec4 v_fragmentPositionDirectionalLightSpace;

out vec3 v_viewPosition;

struct DirectionalLight
{
    vec3 direction;
    bool isActive;
    vec3 color;
};
uniform DirectionalLight u_directionalLight;
out vec3 v_lightDirection;

struct PointLight
{
    vec3 position;
    bool isActive;
    vec3 color;
};
#define NUM_POINT_LIGHTS 1
uniform PointLight u_pointLight[NUM_POINT_LIGHTS];
out vec3 v_lightPosition0;

uniform vec3 u_viewPosition;

uniform mat4 u_modelViewProjMatrix;
uniform mat4 u_modelMatrix;

void main()
{
    gl_Position = u_modelViewProjMatrix * vec4(a_position, 1.0f);

    vec3 T = normalize(vec3(u_modelMatrix * vec4(a_tangent.xyz, 0.0f)));
    vec3 N = normalize(vec3(u_modelMatrix * vec4(a_normal, 0.0f)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * a_tangent.w;
    mat3 TBN = transpose(mat3(T, B, N));

    v_lightDirection = TBN * u_directionalLight.direction;
    v_lightPosition0 = TBN * u_pointLight[0].position;

    v_fragmentPosition = TBN * vec3(u_modelMatrix * vec4(a_position, 1.0f));
    v_viewPosition = TBN * u_viewPosition;

    v_normal = N;
    v_texCoord = a_texCoord;
}
