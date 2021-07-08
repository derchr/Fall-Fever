#include "GLBucket.h"

#include <iostream>

std::unique_ptr<GLBucket> GLBucket::s_instance = std::unique_ptr<GLBucket>(new GLBucket);

GLBucket &GLBucket::instance()
{
    return *s_instance.get();
}

void GLBucket::runGlCall(const std::function<void(void)> &f)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    f();
}
