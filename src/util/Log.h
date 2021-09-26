#pragma once

#include <spdlog/spdlog.h>

class Log
{
public:
    static spdlog::logger &logger();

private:
    Log();

    static Log s_instance;

    std::shared_ptr<spdlog::logger> m_logger;
};