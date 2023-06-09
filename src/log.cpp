#include "log.h"

#include <spdlog/cfg/env.h>

void Log::initialize()
{
    spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::warn);
#endif

    // Override level when running with environment variable.
    spdlog::cfg::load_env_levels();
}
