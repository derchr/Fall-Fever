#pragma once

#include "core/game_loop.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

class Controller : public GameLoop
{
public:
    Controller();
    void update() override;
};
