#pragma once

#include "Light.h"

class Scene {

public:

    Scene(ShaderProgram *shaderProgram);

private:

    ShaderProgram *shaderProgram;

    //DirectionalLight directionalLight;
    PointLight pointLights[NUM_POINT_LIGHTS];
    //SpotLight spotLight;

};