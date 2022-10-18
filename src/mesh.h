#pragma once

#include <cstdint>
#include <glad/gl.h>
#include <map>
#include <variant>
#include <vector>
#include <array>

struct VertexAttributeData
{
    using Scalar = std::vector<float>;
    using Vec2 = std::vector<std::array<float, 2>>;
    using Vec3 = std::vector<std::array<float, 3>>;
    using Vec4 = std::vector<std::array<float, 4>>;

    // Possible improvement:
    // Instead of copying the data into a vector, create a span that
    // points into the original data. For that, the glTF file has to
    // persist until the data is uploaded to the GPU.
    std::variant<Scalar, Vec2, Vec3, Vec4> values;
};

struct Indices
{
    using UnsignedByte = std::vector<uint8_t>;
    using UnsignedShort = std::vector<uint16_t>;
    using UnsignedInt = std::vector<uint32_t>;

    std::variant<UnsignedByte, UnsignedShort, UnsignedInt> values;
};

struct Mesh
{
    using VertexAttributeId = std::size_t;

    std::map<VertexAttributeId, VertexAttributeData> attributes;
    Indices indices;
};

struct GpuMesh
{
    GpuMesh(Mesh const &mesh);

    GpuMesh(GpuMesh const &) = delete;
    auto operator=(GpuMesh const &) -> GpuMesh & = delete;

    GpuMesh(GpuMesh &&other) noexcept : vao(other.vao) { other.vao = 0; }
    auto operator=(GpuMesh &&other) noexcept -> GpuMesh &
    {
        vao = other.vao;
        other.vao = 0;
        return *this;
    };

    ~GpuMesh() { glDeleteVertexArrays(1, &vao); };

    // TODO: also store vertex buffers.
    GLuint vao{};
};
