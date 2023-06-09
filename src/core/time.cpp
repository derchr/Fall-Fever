#include "time.h"
#include <chrono>

void Time::update_delta_time(entt::registry& registry)
{
    auto current_time = std::chrono::system_clock::now();

    auto& delta_time = registry.ctx().emplace<Delta>();
    delta_time.delta = current_time - delta_time.last_time;
    delta_time.last_time = current_time;
}
