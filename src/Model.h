#pragma once

#include "definitions/models.h"

#include <string>
#include <vector>

class ShaderProgram;
class Mesh;
class Texture;

class Model
{
public:
    struct Prototype
    {
        std::string modelName;
        std::string modelPath;
    };

    Model(const Prototype &prototype);
    ~Model();

    void initializeOnGPU();

    void draw(ShaderProgram *shaderProgram);
    void drawWithoutTextures();

    Mesh *getMesh(unsigned int index);
    const std::string &getUniqueName();

private:
    void loadModel(const std::string &pathToModel);

    bool m_isInitialized = false;

    std::vector<Mesh *> m_meshes;
    std::vector<Texture *> m_textures;

    std::string m_workingPath;

    static uint32_t s_idCounter;
    uint32_t m_id;
    std::string m_uniqueName;
};
