#pragma once
#include <stdint.h>

#ifdef __linux__
#include <unistd.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

namespace Helper
{
    static void sleep(uint32_t us) {
#ifdef __linux__
        usleep(us);
#endif
#ifdef _WIN32
        Sleep(us/1000);
#endif
    }
};
