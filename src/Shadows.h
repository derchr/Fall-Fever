#pragma once

#include "Framebuffer.h"

class Shadows {

public:

    Shadows();

    void calculate();

private:

    DepthMap depthMap;

    const int SHADOW_RES = 1024;

};