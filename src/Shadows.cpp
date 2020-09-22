#include "Shadows.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>

Shadows::Shadows() 
    : depthMap(SHADOW_RES) {

}

void Shadows::calculate() {

    glViewport(0, 0, SHADOW_RES, SHADOW_RES);
    depthMap.bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    // ConfigureShaderAndMatrices();
    float near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3( 0.0f, 0.0f,  0.0f), glm::vec3( 0.0f, 1.0f,  0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    // RenderScene();
    depthMap.unbind();

}
