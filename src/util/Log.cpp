#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

Log Log::s_instance;

Log::Log()
{
    m_logger = spdlog::stdout_color_mt("Core");
    m_logger->set_level(spdlog::level::debug);
    // m_logger->set_pattern("");
}

spdlog::logger &Log::logger()
{
    return *s_instance.m_logger;
}
