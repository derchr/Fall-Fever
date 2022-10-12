#pragma once

#include "definitions/attribute_locations.h"

#include <glad/gl.h>
#include <tiny_gltf.h>
#include <vector>

class VertexArray final
{
public:
    VertexArray(tinygltf::Primitive const &primitive, tinygltf::Model const &model, AttributeLocations &locations);
    VertexArray(VertexArray &&other) noexcept
        : m_indicesCount(other.m_indicesCount),
          m_indicesType(other.m_indicesType),
          m_vao(other.m_vao),
          m_positionVbo(other.m_positionVbo),
          m_normalVbo(other.m_normalVbo),
          m_uvVbo(other.m_uvVbo),
          m_tangentVbo(other.m_tangentVbo),
          m_ebo(other.m_ebo)
    {
        other.m_ebo = 0;
        other.m_vao = 0;
        other.m_positionVbo = 0;
        other.m_normalVbo = 0;
        other.m_uvVbo = 0;
        other.m_tangentVbo = 0;
    }

    auto operator=(VertexArray &&other) noexcept -> VertexArray &
    {
        m_indicesCount = other.m_indicesCount;
        m_indicesType = other.m_indicesType;
        m_vao = other.m_vao;
        m_positionVbo = other.m_positionVbo;
        m_normalVbo = other.m_normalVbo;
        m_uvVbo = other.m_uvVbo;
        m_tangentVbo = other.m_tangentVbo;
        m_ebo = other.m_ebo;

        other.m_ebo = 0;
        other.m_vao = 0;
        other.m_positionVbo = 0;
        other.m_normalVbo = 0;
        other.m_uvVbo = 0;
        other.m_tangentVbo = 0;

        return *this;
    }

    VertexArray(VertexArray const &) = delete;
    auto operator=(VertexArray const &) -> VertexArray & = delete;

    ~VertexArray();

    void bind() const;
    static void unbind();

    [[nodiscard]] auto indicesCount() const -> uint64_t { return m_indicesCount; }
    [[nodiscard]] auto indicesType() const -> int { return m_indicesType; }

private:
    uint64_t m_indicesCount;
    int m_indicesType;

    GLuint m_vao;

    GLuint m_positionVbo;
    GLuint m_normalVbo;
    GLuint m_uvVbo;
    GLuint m_tangentVbo;
    GLuint m_ebo;
};
