#include "VertexArray.h"
#include "definitions/models.h"

#include <cstddef>
#include <vector>

VertexArray::VertexArray(tinygltf::Primitive const &primitive, tinygltf::Model const &model,
                         AttributeLocations &locations)
{
    GLuint vao{};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    int position_accessor_id = primitive.attributes.at("POSITION");
    int normal_accessor_id = primitive.attributes.at("NORMAL");
    int uv_accessor_id = primitive.attributes.at("TEXCOORD_0");
    int indices_accessor_id = primitive.indices;

    auto const &position_accessor = model.accessors.at(position_accessor_id);
    auto const &normal_accessor = model.accessors.at(normal_accessor_id);
    auto const &uv_accessor = model.accessors.at(uv_accessor_id);
    auto const &indices_accessor = model.accessors.at(indices_accessor_id);

    int position_buffer_view_id = model.accessors[position_accessor_id].bufferView;
    int normal_buffer_view_id = model.accessors[normal_accessor_id].bufferView;
    int uv_buffer_view_id = model.accessors[uv_accessor_id].bufferView;
    int indices_buffer_view_id = model.accessors[indices_accessor_id].bufferView;

    auto const &position_buffer_view = model.bufferViews.at(position_buffer_view_id);
    auto const &normal_buffer_view = model.bufferViews.at(normal_buffer_view_id);
    auto const &uv_buffer_view = model.bufferViews.at(uv_buffer_view_id);
    auto const &indices_buffer_view = model.bufferViews.at(indices_buffer_view_id);

    auto const &position_buffer = model.buffers.at(position_buffer_view.buffer);
    auto const &normal_buffer = model.buffers.at(normal_buffer_view.buffer);
    auto const &uv_buffer = model.buffers.at(uv_buffer_view.buffer);
    auto const &indices_buffer = model.buffers.at(indices_buffer_view.buffer);

    GLuint positionVbo{};
    {
        glGenBuffers(1, &positionVbo);
        glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
        glBufferData(GL_ARRAY_BUFFER, position_buffer_view.byteLength,
                     position_buffer.data.data() + position_buffer_view.byteOffset, GL_STATIC_DRAW);

        int size = 1;
        if (position_accessor.type == TINYGLTF_TYPE_SCALAR) {
            size = 1;
        } else if (position_accessor.type == TINYGLTF_TYPE_VEC2) {
            size = 2;
        } else if (position_accessor.type == TINYGLTF_TYPE_VEC3) {
            size = 3;
        } else if (position_accessor.type == TINYGLTF_TYPE_VEC4) {
            size = 4;
        } else {
            assert(0);
        }

        int position_byte_stride = position_accessor.ByteStride(position_buffer_view);
        glEnableVertexAttribArray(locations.position);
        glVertexAttribPointer(locations.position, size, position_accessor.componentType,
                              position_accessor.normalized ? GL_TRUE : GL_FALSE, position_byte_stride,
                              (void *)position_accessor.byteOffset);
    }

    GLuint normalVbo{};
    {
        glGenBuffers(1, &normalVbo);
        glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
        glBufferData(GL_ARRAY_BUFFER, normal_buffer_view.byteLength,
                     normal_buffer.data.data() + normal_buffer_view.byteOffset, GL_STATIC_DRAW);

        int size = 1;
        if (normal_accessor.type == TINYGLTF_TYPE_SCALAR) {
            size = 1;
        } else if (normal_accessor.type == TINYGLTF_TYPE_VEC2) {
            size = 2;
        } else if (normal_accessor.type == TINYGLTF_TYPE_VEC3) {
            size = 3;
        } else if (normal_accessor.type == TINYGLTF_TYPE_VEC4) {
            size = 4;
        } else {
            assert(0);
        }

        int normal_byte_stride = normal_accessor.ByteStride(normal_buffer_view);
        glEnableVertexAttribArray(locations.normal);
        glVertexAttribPointer(locations.normal, size, normal_accessor.componentType,
                              normal_accessor.normalized ? GL_TRUE : GL_FALSE, normal_byte_stride,
                              (void *)normal_accessor.byteOffset);
    }

    GLuint uvVbo{};
    {
        glGenBuffers(1, &uvVbo);
        glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
        glBufferData(GL_ARRAY_BUFFER, uv_buffer_view.byteLength,
                     uv_buffer.data.data() + uv_buffer_view.byteOffset, GL_STATIC_DRAW);

        int size = 1;
        if (uv_accessor.type == TINYGLTF_TYPE_SCALAR) {
            size = 1;
        } else if (uv_accessor.type == TINYGLTF_TYPE_VEC2) {
            size = 2;
        } else if (uv_accessor.type == TINYGLTF_TYPE_VEC3) {
            size = 3;
        } else if (uv_accessor.type == TINYGLTF_TYPE_VEC4) {
            size = 4;
        } else {
            assert(0);
        }

        int uv_byte_stride = uv_accessor.ByteStride(uv_buffer_view);
        glEnableVertexAttribArray(locations.uv);
        glVertexAttribPointer(locations.uv, size, uv_accessor.componentType,
                              uv_accessor.normalized ? GL_TRUE : GL_FALSE, uv_byte_stride,
                              (void *)uv_accessor.byteOffset);
    }

    GLuint ebo{};
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_view.byteLength,
                 indices_buffer.data.data() + indices_buffer_view.byteOffset, GL_STATIC_DRAW);

    glBindVertexArray(0);

    m_vao = vao;
    m_ebo = ebo;
    m_positionVbo = positionVbo;
    m_normalVbo = normalVbo;
    m_uvVbo = uvVbo;
    m_indicesCount = indices_accessor.count;
    m_indicesType = indices_accessor.componentType;
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_vao);

    glDeleteBuffers(1, &m_positionVbo);
    glDeleteBuffers(1, &m_normalVbo);
    glDeleteBuffers(1, &m_uvVbo);
    glDeleteBuffers(1, &m_ebo);
}

void VertexArray::bind() const
{
    glBindVertexArray(m_vao);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}
