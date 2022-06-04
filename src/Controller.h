#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class ShaderProgram;
class Window;
class EventHandler;
class Scene;
class Camera;
class Menu;
class FrameBuffer;

namespace Imgui {
class Handler;
}

class Controller
{
public:
    Controller();
    ~Controller();

    void run();

    void setMaxFps(uint16_t fps);

    // TODO remove...
    ShaderProgram *getShaderProgramByName(const std::string &name);
    static ShaderProgram *getShaderProgramByName(const std::string &name, std::vector<ShaderProgram *> shaderPrograms);

    void updateExposure(ShaderProgram *shaderProgram);

private:
    void limit_framerate();

    void updateWindowDimensions();

    void renderImGui(Scene *world, glm::vec3 *lightColor, bool *rotateEntity, bool *rotateLightSource,
                     ShaderProgram *postProcessingProgram, float *intensity, bool *drawShadows);

    std::unique_ptr<Window> m_gameWindow;
    EventHandler *m_gameEventHandler;

    Scene *m_scene;

    Camera *m_camera;

    std::vector<ShaderProgram *> m_shaderPrograms;

    FrameBuffer *m_postProcessFrameBuffer;

    uint16_t m_MAX_FPS = 60;
    double m_deltaTime;

    float m_exposure = 1.0f;
};
