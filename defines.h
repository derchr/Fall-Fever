 #pragma once

#define INIT_WINDOW_WIDTH 960
#define INIT_WINDOW_HEIGHT 720 
struct Vertex {
    // Postition
    float x;
    float y;
    float z;
    
    // UV Texture Mapping
    float u;
    float v;

    // Color
    float r;
    float g;
    float b;
    float a;
};
