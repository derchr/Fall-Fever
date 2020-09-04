#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures)
    : vertices(vertices),
      indices(indices),
      textures(textures),
      vertexBuffer(static_cast<void*>(vertices.data()), static_cast<void*>(indices.data()), vertices.size(), indices.size()) {


}

void Mesh::draw(ShaderProgram *shaderProgram) {

    uint8_t typeNumberCount[TEXTURE_TYPE_NUM_ITEMS] {0};

    // Bind all textures in order to its texture unit
    for(auto it = textures.begin(); it != textures.end(); it++) {
        int i = it - textures.begin();

        uint8_t currentTextureType = (*it).getTextureType();

        (*it).bind(i, shaderProgram, typeNumberCount[currentTextureType]);

        typeNumberCount[currentTextureType] += 1;
    }
    
    // Draw elements
    vertexBuffer.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vertexBuffer.unbind();

    // Unbind all textures
    for(auto it = textures.begin(); it != textures.end(); it++) {
        (*it).unbind();
    }
    

}
