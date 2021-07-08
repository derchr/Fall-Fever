#pragma once

#include <functional>
#include <memory>
#include <mutex>

class GLBucket
{
public:
    static GLBucket &instance();

    void runGlCall(const std::function<void(void)> &f);

private:
    GLBucket() = default;

    std::mutex m_mutex;
    static std::unique_ptr<GLBucket> s_instance;
};