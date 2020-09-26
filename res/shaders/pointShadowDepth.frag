#version 330 core
in vec4 v_fragmentPosition;

uniform vec3 v_lightPos;
uniform float pointShadowDepthMapFarPlane;

void main() {

    // Distance between fragment and light source
    float lightDistance = length(v_fragmentPosition.xyz - v_lightPos);
    
    // map to [0;1] range
    lightDistance = lightDistance / pointShadowDepthMapFarPlane;
    
    gl_FragDepth = lightDistance;

} 