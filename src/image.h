#pragma once

#include <filesystem>
#include <glad/gl.h>
#include <span>
#include <vector>

struct Sampler
{
    enum class MagFilter : uint16_t
    {
        None,
        Nearest = 9728,
        Linear = 9729
    };

    enum class MinFilter : uint16_t
    {
        None,
        Nearest = 9728,
        Linear = 9729,
        NearestMipMapNearest = 9984,
        LinearMipMapNearest = 9985,
        NearestMipMapLinear = 9986,
        LinearMipMapLinear = 9987
    };

    enum class WrappingMode : uint16_t
    {
        ClampToEdge = 33071,
        MirroredRepeat = 33648,
        Repeat = 10497
    };

    MagFilter magFilter = MagFilter::Linear;
    MinFilter minFilter = MinFilter::LinearMipMapLinear;
    WrappingMode wrapS = WrappingMode::Repeat;
    WrappingMode wrapT = WrappingMode::Repeat;
};

struct Image
{
    std::vector<uint8_t> data;
    Sampler sampler;

    struct Extent
    {
        unsigned int width;
        unsigned int height;
    } extent{};

    enum class DataFormat
    {
        R8Uint,
        RGB8Uint,
        RGBA8Uint,
    } dataFormat;

    enum class ColorFormat
    {
        SRGB,
        RGB
    } colorFormat;

    Image(std::span<uint8_t const> bytes, ColorFormat colorFormat);
};

struct GpuImage
{
    static constexpr float LOD_BIAS = -2.0;

    GpuImage(Image const &image);

    GpuImage(GpuImage const &) = delete;
    auto operator=(GpuImage const &) -> GpuImage & = delete;

    GpuImage(GpuImage &&other) noexcept : texture(other.texture) { other.texture = 0; }
    auto operator=(GpuImage &&other) noexcept -> GpuImage &
    {
        texture = other.texture;
        other.texture = 0;
        return *this;
    };

    ~GpuImage() { glDeleteTextures(1, &texture); };

    GLuint texture{};
};
