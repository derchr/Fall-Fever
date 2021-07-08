#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class ShaderProgram;
class Window;
class EventHandler;
class World;
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

    static ShaderProgram *getShaderProgramByName(std::vector<ShaderProgram *> shaderPrograms, const std::string &name);

    void updateExposure(ShaderProgram *shaderProgram);

private:
    void limit_framerate();

    void updateWindowDimensions();

    ShaderProgram *getShaderProgramByName(const std::string &name);

    void renderImGui(World *world, glm::vec3 *lightColor, bool *rotateEntity, bool *rotateLightSource,
                     ShaderProgram *postProcessingProgram, float *intensity, bool *drawShadows);

    std::unique_ptr<Window> m_gameWindow;
    EventHandler *m_gameEventHandler;

    World *m_world;

    Camera *m_camera;
    Menu *m_menu;

    std::vector<ShaderProgram *> m_shaderPrograms;

#ifdef _DEBUG
    std::unique_ptr<Imgui::Handler> m_imguiHandler;
#endif

    FrameBuffer *m_postProcessFrameBuffer;

    uint16_t m_MAX_FPS = 60;
    double m_deltaTime;

    float m_exposure = 1.0f;
};
