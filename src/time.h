#pragma once

#include <chrono>

namespace Time {

struct Delta
{
    std::chrono::duration<float> delta;
};

} // namespace Time