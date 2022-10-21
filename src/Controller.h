#pragma once

#include "FrameBuffer.h"
#include "ShaderProgram.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

class Window;
class Scene;
class Camera;
class Framebuffer;

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
    ShaderProgram skyboxProgram{{"skyboxProgram", "data/shaders/skybox.vert", "data/shaders/skybox.frag"}};
    ShaderProgram postProcessingProgram{
        {"postProcessingProgram", "data/shaders/postprocessing.vert", "data/shaders/postprocessing.frag"}};

    Framebuffer m_postProcessFrameBuffer;

    static constexpr unsigned MAX_FPS = 60;

    Scene m_scene;

    double m_deltaTime{};
    float m_exposure = 1.0;
};
