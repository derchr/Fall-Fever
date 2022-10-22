#pragma once

#include "FrameBuffer.h"
#include "Scene.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
#include <entt/entt.hpp>

class Window;
class Scene;
class Camera;
class Framebuffer;

class Controller
{
public:
    Controller();

    void run();

    void updateExposure(Shader &shader) const;

private:
    void limit_framerate();
    void update_window_dimensions();

    std::shared_ptr<Window> m_gameWindow;
    
    Shader skybox_shader{"skybox", "data/shaders"};
    Shader post_processing_shader{"post_processing", "data/shaders"};

    Framebuffer m_postProcessFrameBuffer;

    entt::resource_cache<Shader, ShaderLoader> m_shader_cache;

    static constexpr unsigned MAX_FPS = 60;

    Scene m_scene;

    double m_deltaTime{};
    float m_exposure = 1.0;
};
