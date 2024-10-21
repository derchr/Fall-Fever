#include "controller.h"
#include "components/visibility.h"
#include "flycam.h"

#include "components/name.h"
#include "components/transform.h"
#include "core/camera.h"
#include "core/light.h"
#include "window/window.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

using namespace entt::literals;

Controller::Controller(std::string_view path)
{
    spdlog::info("Open {}", path);
    std::filesystem::path document_path(path);
    entt::hashed_string document_hash(document_path.string().c_str());

    entt::resource<Gltf> gltf_document =
        gltf_cache.load(document_hash, document_path).first->second;

    gltf_document->spawn_default_scene(registry(), gltf_node_cache);

    // Spawn default lights
    auto directional_light = registry().create();
    registry().emplace<Name>(directional_light, "Directional Light");
    registry().emplace<Transform>(
        directional_light,
        Transform{.orientation = glm::toQuat(
                      glm::lookAt({}, DirectionalLight::DEFAULT_DIRECTION, Camera::UP_VECTOR))});
    registry().emplace<GlobalTransform>(directional_light, GlobalTransform{});
    registry().emplace<DirectionalLight>(
        directional_light, DirectionalLight{.illuminance = DirectionalLight::DEFAULT_ILLUMINANCE});

    auto point_light = registry().create();
    registry().emplace<Name>(point_light, "Point Light");
    registry().emplace<Transform>(point_light,
                                  Transform{.translation = PointLight::DEFAULT_POSITION});
    registry().emplace<GlobalTransform>(point_light, GlobalTransform{});
    registry().emplace<PointLight>(point_light,
                                   PointLight{.intensity = PointLight::DEFAULT_INTENSITY});

    // Spawn default camera
    auto camera_view = registry().view<Camera const>();
    if (camera_view.empty()) {
        auto entity = registry().create();
        registry().emplace<Name>(entity, "Camera");
        registry().emplace<Transform>(entity, Transform{.translation = glm::vec3(0.0, 0.25, -1.0)});
        registry().emplace<GlobalTransform>(entity, GlobalTransform{});
        registry().emplace<Camera>(entity, Camera{.projection = Camera::Perspective{}});
        registry().emplace<Flycam>(entity);
    }

    // imgui
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(&this->game_window->handle(), true);
    ImGui_ImplOpenGL3_Init("#version 330"); // Use appropriate GLSL version
}

void Controller::update()
{
    Flycam::keyboard_movement(registry());

    if (registry().ctx().get<Window::MouseCatched>().catched) {
        Flycam::mouse_orientation(registry());
    }
}

void Controller::renderUi()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    auto name_view = registry().view<Name const, Visibility>();
    for (auto [entity, name, visibility] : name_view.each()) {
        bool visible = visibility.entity_visibility != Visibility::EntityVisibility::Hidden;
        ImGui::Checkbox(name.name.c_str(), &visible);

        visibility.entity_visibility = visible ? Visibility::EntityVisibility::Inherited
                                               : Visibility::EntityVisibility::Hidden;
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
