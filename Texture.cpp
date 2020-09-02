#include "Texture.h"

#include <stb/stb_image.h>
#include <iostream>

Texture::Texture(const char* texturePath, GLuint shaderProgramId) {
    this->shaderProgramId = shaderProgramId;

    stbi_set_flip_vertically_on_load(1);
    stbi_uc *textureBuffer = stbi_load(texturePath, &textureWidth, &textureHeight, &bitsPerPixel, 4);

    // Push texture to grahics card;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


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

void Texture::bind(uint8_t textureUnit) {
    GLint uniformLoc = glGetUniformLocation(shaderProgramId, "u_texture");
    glUniform1i(uniformLoc, textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
