#include "Scene.h"

Scene::Scene(ShaderProgram *shaderProgram)
    : shaderProgram(shaderProgram), pointLights(shaderProgram) {

}
