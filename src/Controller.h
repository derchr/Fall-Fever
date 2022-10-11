#pragma once

#include "FrameBuffer.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "resources/Model.h"
#include "Entity.h"
#include "resources/Texture.h"

#include <glm/glm.hpp>
#include <memory>
#include <tiny_gltf.h>
#include <vector>

class Window;
class Scene;
class Camera;
class FrameBuffer;

class Controller
{
public:
    Controller();

    void run();

    void updateExposure(ShaderProgram &shaderProgram) const;

private:
    void limit_framerate();
    void update_window_dimensions();

    std::shared_ptr<Window> m_gameWindow;

    std::shared_ptr<Camera> m_camera;

    ShaderProgram defaultProgram{{"defaultProgram", "data/shaders/basic.vert", "data/shaders/basic.frag"}};
    ShaderProgram lightProgram{{"lightProgram", "data/shaders/light.vert", "data/shaders/light.frag"}};
    ShaderProgram skyboxProgram{{"skyboxProgram", "data/shaders/skybox.vert", "data/shaders/skybox.frag"}};
    ShaderProgram postProcessingProgram{
        {"postProcessingProgram", "data/shaders/postprocessing.vert", "data/shaders/postprocessing.frag"}};

    FrameBuffer m_postProcessFrameBuffer;

    static constexpr unsigned MAX_FPS = 60;

    tinygltf::Model m_model;

    std::vector<ModelEntity> m_entities;
    std::vector<Model> m_models;
    std::vector<Texture> m_textures;

    double m_deltaTime{};
    float m_exposure = 1.0;
};
