#pragma once

#include "core/application.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

class Controller : public FeverCore::Application
{
public:
    Controller(std::string_view path);
    void update() override;
};
