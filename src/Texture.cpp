#include "Texture.h"

#include <stb/stb_image.h>
#include <iostream>

Texture::Texture(const char* texturePath, uint8_t textureType) {

    this->texturePath = texturePath;
    this->textureType = textureType;

    stbi_set_flip_vertically_on_load(1);
    auto *textureBuffer = stbi_load(texturePath, &textureWidth, &textureHeight, &bitsPerPixel, STBI_rgb_alpha);

    // Push texture to grahics card;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    if(textureBuffer) {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);
        //glGenerateMipmap(GL_TEXTURE_2D);

    } else {

        std::cout << "[Warning] Texture " << texturePath << " not found!" << std::endl;

    }

    stbi_image_free(textureBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);

}

Texture::~Texture() {
    glDeleteTextures(1, &textureId);
}

void Texture::bind(uint8_t textureUnit, ShaderProgram* shaderProgram, uint8_t textureTypeNum) {
    std::string uniformName = "texture_";

    switch(textureType) {

        case texture_diffuse:
            uniformName += "diffuse" + std::to_string(textureTypeNum);
            break;
        case texture_specular:
            uniformName += "specular" + std::to_string(textureTypeNum);
            break;
        case texture_normal:
            uniformName += "normal" + std::to_string(textureTypeNum);
            break;
        case texture_height:
            uniformName += "height" + std::to_string(textureTypeNum);
            break;
        case texture_gloss:
            uniformName += "gloss" + std::to_string(textureTypeNum);
            break;
    }

    // Add u_material as we store textures in a struct
    uniformName = "u_material." + uniformName;

    shaderProgram->setUniform(uniformName.c_str(), textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
