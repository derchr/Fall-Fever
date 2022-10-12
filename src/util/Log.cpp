#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/cfg/env.h>

Log Log::s_instance;

Log::Log() noexcept
{
    m_logger = spdlog::stdout_color_mt("Core");
    m_logger->set_pattern("[%H:%M:%S.%e] [%n] [%^%l%$] %v");

#ifndef NDEBUG
    m_logger->set_level(spdlog::level::debug);
#else
    m_logger->set_level(spdlog::level::warn);
#endif

    // Override level when running with environment variable.
    spdlog::cfg::load_env_levels();
}

spdlog::logger &Log::logger()
{
    return *s_instance.m_logger;
}
