#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class ShaderProgram;
class Window;
class Scene;
class Camera;
class FrameBuffer;

class Controller
{
public:
    Controller();

    void run();

    // TODO remove...
    std::shared_ptr<ShaderProgram> getShaderProgramByName(const std::string &name);
    static std::shared_ptr<ShaderProgram>
    getShaderProgramByName(const std::string &name, std::vector<std::shared_ptr<ShaderProgram>> shaderPrograms);

    void updateExposure(ShaderProgram &shaderProgram) const;

private:
    void limit_framerate();
    void update_window_dimensions();

    std::shared_ptr<Window> m_gameWindow;
    std::shared_ptr<Scene> m_scene;

    std::shared_ptr<Camera> m_camera;

    std::vector<std::shared_ptr<ShaderProgram>> m_shaderPrograms;

    std::shared_ptr<FrameBuffer> m_postProcessFrameBuffer;

    static constexpr unsigned MAX_FPS = 60;

    double m_deltaTime{};
    float m_exposure = 1.0;
};
