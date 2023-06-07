#pragma once

#include <chrono>

namespace Time {

struct Delta
{
    std::chrono::duration<double> delta;
};

} // namespace Time