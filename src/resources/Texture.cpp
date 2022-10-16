#include "Texture.h"
#include "../ShaderProgram.h"
#include "../util/Log.h"

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(fx::gltf::Texture const &texture,
                 std::filesystem::path const &working_directory,
                 std::span<fx::gltf::Image> images,
                 std::span<fx::gltf::BufferView> bufferViews,
                 std::span<fx::gltf::Buffer> buffers,
                 std::span<fx::gltf::Sampler> samplers,
                 TextureType textureType)
    : m_textureType(textureType)
{
    auto sampler_id = texture.sampler;

    auto sampler = [samplers, sampler_id]() {
        if (sampler_id != -1) {
            return samplers[static_cast<unsigned>(sampler_id)];
        }

        return fx::gltf::Sampler{.magFilter = fx::gltf::Sampler::MagFilter::Linear,
                                 .minFilter = fx::gltf::Sampler::MinFilter::LinearMipMapLinear,
                                 .wrapS = fx::gltf::Sampler::WrappingMode::Repeat,
                                 .wrapT = fx::gltf::Sampler::WrappingMode::Repeat};
    }();

    auto const &image = images[static_cast<unsigned>(texture.source)];
    auto const &imageBufferView = bufferViews[static_cast<unsigned>(image.bufferView)];
    auto const &imageBuffer = buffers[static_cast<unsigned>(imageBufferView.buffer)];

    int width{};
    int height{};
    int components{};

    auto *stbi_image = [&]() {
        if (!image.uri.empty()) {
            auto image_path = working_directory / image.uri;
            return stbi_load(image_path.c_str(), &width, &height, &components, 0);
        }

        return stbi_load_from_memory(&imageBuffer.data[imageBufferView.byteOffset],
                                     static_cast<int>(imageBufferView.byteOffset),
                                     &width,
                                     &height,
                                     &components,
                                     0);
    }();

    GLenum internalFormat{};
    GLenum dataFormat{};

    switch (components) {
    case 1:
        internalFormat = GL_RED;
        dataFormat = GL_RED;
        break;
    case 3:
        internalFormat = (m_textureType == TextureType::Diffuse) ? GL_SRGB8 : GL_RGB8;
        dataFormat = GL_RGB;
        break;
    case 4:
        internalFormat = (m_textureType == TextureType::Diffuse) ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        dataFormat = GL_RGBA;
        break;
    }

    glGenTextures(1, &m_glId);
    glBindTexture(GL_TEXTURE_2D, m_glId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(sampler.magFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(sampler.minFilter));
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -2.0F);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(sampler.wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(sampler.wrapT));

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 static_cast<GLint>(internalFormat),
                 static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height),
                 0,
                 dataFormat,
                 GL_UNSIGNED_BYTE,
                 stbi_image);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(stbi_image);

    Log::logger().trace(R"(Loaded texture "{}")", image.name);
}

auto Texture::textureType() const -> TextureType
{
    return m_textureType;
}

void Texture::bind(uint8_t textureUnit, ShaderProgram const &shaderProgram) const
{
    std::string uniformName = "texture_";

    switch (m_textureType) {
    case TextureType::Diffuse:
        uniformName += "diffuse";
        break;
    case TextureType::Normal:
        uniformName += "normal";
        break;
    default:
        break;
    }

    // Add u_material as we store textures in a struct
    uniformName = "u_material." + uniformName;

    shaderProgram.setUniform(uniformName, textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_glId);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
