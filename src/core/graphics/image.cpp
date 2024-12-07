#include "image.h"

#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image::Image(std::span<uint8_t const> bytes, ColorFormat colorFormat) : colorFormat(colorFormat)
{
    int width{};
    int height{};
    int components{};

    uint8_t* stbi_image = stbi_load_from_memory(
        bytes.data(), static_cast<int>(bytes.size()), &width, &height, &components, 0);

    std::size_t const buffer_length = static_cast<unsigned>(width * height * components);

    // Copy the image data into a vector as stbi currently does not support writing into
    // user-defined buffers.
    std::copy(
        stbi_image,
        &stbi_image[buffer_length], // NOLINT (cppcoreguidelines-pro-bounds-pointer-arithmetic)
        std::back_inserter(data));

    dataFormat = [components]() {
        switch (components) {
        case 1:
            return DataFormat::R8Uint;

        case 3:
            return DataFormat::RGB8Uint;

        case 4:
            return DataFormat::RGBA8Uint;

        default:
            spdlog::warn("Unsupported data format for image.");
            return DataFormat::RGBA8Uint;
        }
    }();

    extent = Extent{.width = static_cast<unsigned>(width), .height = static_cast<unsigned>(height)};

    stbi_image_free(stbi_image);
}

GpuImage::GpuImage(Image const& image)
{
    GLenum internalFormat{};
    GLenum dataFormat{};

    switch (image.dataFormat) {
    case Image::DataFormat::R8Uint:
        internalFormat = GL_RED;
        dataFormat = GL_RED;
        break;
    case Image::DataFormat::RGB8Uint:
        internalFormat = (image.colorFormat == Image::ColorFormat::SRGB) ? GL_SRGB8 : GL_RGB8;
        dataFormat = GL_RGB;
        break;
    case Image::DataFormat::RGBA8Uint:
        internalFormat =
            (image.colorFormat == Image::ColorFormat::SRGB) ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        dataFormat = GL_RGBA;
        break;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(image.sampler.magFilter));
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(image.sampler.minFilter));
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, LOD_BIAS);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(image.sampler.wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(image.sampler.wrapT));

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 static_cast<GLint>(internalFormat),
                 static_cast<GLsizei>(image.extent.width),
                 static_cast<GLsizei>(image.extent.height),
                 0,
                 dataFormat,
                 GL_UNSIGNED_BYTE,
                 image.data.data());

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}
