#pragma once

#include "core/game_loop.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

class Controller : public GameLoop
{
public:
    Controller(std::string_view path);
    void update() override;
};
