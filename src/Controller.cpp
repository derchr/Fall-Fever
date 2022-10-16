#include "Controller.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include "Helper.h"
#include "Light.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Window.h"
#include "definitions/attribute_locations.h"
#include "gltf_loader.h"
#include "resources/Model.h"
#include "util/Log.h"

#include <GLFW/glfw3.h>
#include <array>
#include <filesystem>
#include <fx/gltf.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <utility>

struct MyRes
{
    MyRes(std::filesystem::path const &path, int)
    {
        std::cout << "Path: " << std::filesystem::weakly_canonical(path) << std::endl;
    };
    int x = 0;
};
using namespace entt::literals;

Controller::Controller()
    : m_gameWindow(std::make_shared<Window>()),
      m_postProcessFrameBuffer(
          m_gameWindow->dimensions().first, m_gameWindow->dimensions().second, postProcessingProgram)
{
    gltf_loader<fx::gltf::Document>()("Lantern/glTF-Binary/Lantern.glb");

    fx::gltf::ReadQuotas read_quotas{.MaxFileSize = 512 * 1024 * 1024, .MaxBufferByteLength = 512 * 1024 * 1024};

    // auto gltf_path = std::filesystem::path("Lantern/glTF/Lantern.gltf");
    auto gltf_path = std::filesystem::path("WaterBottle/glTF/WaterBottle.gltf");
    auto gltf = [&]() {
        if (gltf_path.extension() == ".gltf") {
            return fx::gltf::LoadFromText(gltf_path, read_quotas);
        }

        return fx::gltf::LoadFromBinary(gltf_path, read_quotas);
    }();

    defaultProgram.bind();
    AttributeLocations locations{};

    locations.position = glGetAttribLocation(defaultProgram.getShaderProgramId(), "a_position");
    locations.normal = glGetAttribLocation(defaultProgram.getShaderProgramId(), "a_normal");
    locations.uv = glGetAttribLocation(defaultProgram.getShaderProgramId(), "a_texCoord");
    locations.tangent = glGetAttribLocation(defaultProgram.getShaderProgramId(), "a_tangent");

    ShaderProgram::unbind();

    if (!gltf.cameras.empty()) {
        auto const &gltf_camera = gltf.cameras.at(0);

        assert(gltf_camera.type == fx::gltf::Camera::Type::Perspective);
        auto const &perspective = gltf_camera.perspective;

        m_camera = std::make_shared<Camera>(perspective.yfov, perspective.aspectRatio);
    } else {
        m_camera = std::make_shared<Camera>(90., m_gameWindow->aspectRatio());
    }

    std::vector<Model> models;
    for (auto const &mesh : gltf.meshes) {
        std::vector<Mesh> meshes;
        for (auto const &primitive : mesh.primitives) {
            auto const &material = gltf.materials.at(primitive.material);
            auto baseColorTexture = material.pbrMetallicRoughness.baseColorTexture.index;
            auto normalTexture = material.normalTexture.index;

            std::vector<std::reference_wrapper<const Texture>> primitive_textures;

            // Check if texture already exists, if not load it.
            if (baseColorTexture != -1 && !m_textures.contains(baseColorTexture)) {
                auto const &gltf_texture = gltf.textures.at(baseColorTexture);
                m_textures.emplace(baseColorTexture,
                                   Texture(gltf_texture,
                                           gltf_path.parent_path(),
                                           gltf.images,
                                           gltf.bufferViews,
                                           gltf.buffers,
                                           gltf.samplers,
                                           TextureType::Diffuse));

                primitive_textures.emplace_back(m_textures.at(baseColorTexture));
            }

            if (normalTexture != -1 && !m_textures.contains(normalTexture)) {
                auto const &gltf_texture = gltf.textures.at(normalTexture);
                m_textures.emplace(normalTexture,
                                   Texture(gltf_texture,
                                           gltf_path.parent_path(),
                                           gltf.images,
                                           gltf.bufferViews,
                                           gltf.buffers,
                                           gltf.samplers,
                                           TextureType::Normal));

                primitive_textures.emplace_back(m_textures.at(normalTexture));
            }

            meshes.emplace_back(Mesh({primitive, gltf, locations}, primitive_textures));
        }
        models.emplace_back(Model(mesh.name, std::move(meshes)));
    }
    m_models = std::move(models);

    std::vector<ModelEntity> entities;
    for (auto const &node : gltf.nodes) {
        if (node.mesh == -1) {
            continue;
        }

        ModelEntity entity(
            Entity::Prototype(node.name, {}, {}, 1.0F), m_models[static_cast<unsigned>(node.mesh)], defaultProgram);

        if (!node.translation.empty()) {
            entity.setPosition(glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
        }

        if (!node.rotation.empty()) {
            entity.setRotation(
                glm::eulerAngles(glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2])));
        }

        if (!node.scale.empty()) {
            entity.setScale(node.scale[0]);
        }

        entities.push_back(std::move(entity));

        for (auto const &child : node.children) {
            if (!node.translation.empty()) {
                entities[child].translate(glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
            }
        }
    }
    m_entities = std::move(entities);
}

void Controller::run()
{
    updateExposure(postProcessingProgram);

    m_camera->translate(glm::vec3(0., .5, 2.));
    DirectionalLight directional_light(
        DirectionalLight::Prototype("", glm::vec3(-0.2, -1.0, -0.3), glm::vec3(1.0f), 5.f), &defaultProgram);
    directional_light.setActive(true);

    PointLight point_light(PointLight::Prototype("", "", glm::vec3(4.0, 1.0, 6.0), glm::vec3(1.0F), 3.0F),
                           &defaultProgram);
    point_light.setActive(true);

    Log::logger().info("Startup complete. Enter game loop.");

    // This is the game loop
    while (glfwWindowShouldClose(&m_gameWindow->glfw_window()) == GLFW_FALSE) {
        // --- Timing ---
        limit_framerate();

        // --- Update game ---

        // --- Render and buffer swap ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_postProcessFrameBuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_camera->lookForward();
        m_camera->updateVPM();

        // Draw scene
        defaultProgram.bind();
        for (auto const &entity : m_entities) {
            entity.draw(m_camera->getViewProj(), m_camera->getPosition());
        }
        ShaderProgram::unbind();

        m_postProcessFrameBuffer.unbind();
        m_postProcessFrameBuffer.drawOnEntireScreen();

        glfwSwapBuffers(&m_gameWindow->glfw_window());

        // Update window size
        if (m_gameWindow->dimensions_changed()) {
            m_gameWindow->update_dimensions();
            update_window_dimensions();
        }

        // --- Check events, handle input ---
        m_gameWindow->clear_mouse_cursor_input();
        glfwPollEvents();

        auto const &key_input = m_gameWindow->key_input();
        auto const &mouse_cursor_input = m_gameWindow->mouse_cursor_input();

        m_camera->updatePositionFromKeyboardInput(key_input, (float)m_deltaTime);

        if (m_gameWindow->cursor_catched()) {
            m_camera->updateDirectionFromMouseInput(mouse_cursor_input);
        }
    }
}

void Controller::limit_framerate()
{
    static double startingTime = 0.0;
    static double lastTime = 0.0;

    lastTime = glfwGetTime() - startingTime;

    double frameTime = 1 / (double)MAX_FPS;
    if (frameTime > lastTime) {
        Helper::sleep(static_cast<unsigned>(frameTime - lastTime) * 1000000);
    }

    m_deltaTime = glfwGetTime() - startingTime;

    startingTime = glfwGetTime();
}

void Controller::update_window_dimensions()
{
    m_camera->updateAspectRatio(m_gameWindow->aspectRatio());
    // m_gameEventHandler->setFirstMouseInput(1);

    auto dimensions = m_gameWindow->dimensions();
    m_postProcessFrameBuffer.changeDimensions(dimensions.first, dimensions.second);
}

void Controller::updateExposure(ShaderProgram &shaderProgram) const
{
    shaderProgram.bind();
    shaderProgram.setUniform("u_exposure", m_exposure);
    shaderProgram.unbind();
}
