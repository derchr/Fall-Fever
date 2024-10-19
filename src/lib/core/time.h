#pragma once

#include <chrono>
#include <entt/entt.hpp>

namespace Time {

struct Delta
{
    std::chrono::duration<double> delta;
    std::chrono::time_point<std::chrono::system_clock> last_time;
};

void update_delta_time(entt::registry& registry);

} // namespace Time