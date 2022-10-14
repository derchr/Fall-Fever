#include "VertexArray.h"
#include "definitions/models.h"
#include "util/Log.h"

#include <cstddef>
#include <vector>

static auto accessor_byte_size(fx::gltf::Accessor::Type accessor_type) -> std::size_t
{
    switch (accessor_type) {
    case fx::gltf::Accessor::Type::Scalar:
        return 1;
    case fx::gltf::Accessor::Type::Vec2:
        return 2;
    case fx::gltf::Accessor::Type::Vec3:
        return 3;
    case fx::gltf::Accessor::Type::Vec4:
        return 4;
    default:
        Log::logger().warn("Unexpected accessor type: {}", static_cast<int>(accessor_type));
        return 0;
    }
}

VertexArray::VertexArray(fx::gltf::Primitive const &primitive,
                         fx::gltf::Document const &model,
                         AttributeLocations &locations)
{
    GLuint vao{};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    if (!primitive.attributes.contains("TANGENT") || !primitive.attributes.contains("NORMAL")) {
        Log::logger().critical("glTF scene has to include tangent and normal components!");
        std::terminate();
    }

    int position_accessor_id = primitive.attributes.at("POSITION");
    int normal_accessor_id = primitive.attributes.at("NORMAL");
    int uv_accessor_id = primitive.attributes.at("TEXCOORD_0");
    int tangent_accessor_id = primitive.attributes.at("TANGENT");
    int indices_accessor_id = primitive.indices;

    auto const &position_accessor = model.accessors.at(position_accessor_id);
    auto const &normal_accessor = model.accessors.at(normal_accessor_id);
    auto const &uv_accessor = model.accessors.at(uv_accessor_id);
    auto const &tangent_accessor = model.accessors.at(tangent_accessor_id);
    auto const &indices_accessor = model.accessors.at(indices_accessor_id);

    int position_buffer_view_id = model.accessors[position_accessor_id].bufferView;
    int normal_buffer_view_id = model.accessors[normal_accessor_id].bufferView;
    int uv_buffer_view_id = model.accessors[uv_accessor_id].bufferView;
    int tangent_buffer_view_id = model.accessors[tangent_accessor_id].bufferView;
    int indices_buffer_view_id = model.accessors[indices_accessor_id].bufferView;

    auto const &position_buffer_view = model.bufferViews.at(position_buffer_view_id);
    auto const &normal_buffer_view = model.bufferViews.at(normal_buffer_view_id);
    auto const &uv_buffer_view = model.bufferViews.at(uv_buffer_view_id);
    auto const &tangent_buffer_view = model.bufferViews.at(tangent_buffer_view_id);
    auto const &indices_buffer_view = model.bufferViews.at(indices_buffer_view_id);

    auto const &position_buffer = model.buffers.at(position_buffer_view.buffer);
    auto const &normal_buffer = model.buffers.at(normal_buffer_view.buffer);
    auto const &uv_buffer = model.buffers.at(uv_buffer_view.buffer);
    auto const &tangent_buffer = model.buffers.at(tangent_buffer_view.buffer);
    auto const &indices_buffer = model.buffers.at(indices_buffer_view.buffer);

    GLuint positionVbo{};
    {
        glGenBuffers(1, &positionVbo);
        glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
        glBufferData(GL_ARRAY_BUFFER,
                     position_buffer_view.byteLength,
                     position_buffer.data.data() + position_buffer_view.byteOffset,
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(locations.position);
        glVertexAttribPointer(locations.position,
                              accessor_byte_size(position_accessor.type),
                              static_cast<GLenum>(position_accessor.componentType),
                              position_accessor.normalized ? GL_TRUE : GL_FALSE,
                              position_buffer_view.byteStride,
                              reinterpret_cast<void *>(position_accessor.byteOffset)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast,
                                                                                       // performance-no-int-to-ptr)
    }

    GLuint normalVbo{};
    {
        glGenBuffers(1, &normalVbo);
        glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
        glBufferData(GL_ARRAY_BUFFER,
                     normal_buffer_view.byteLength,
                     normal_buffer.data.data() + normal_buffer_view.byteOffset,
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(locations.normal);
        glVertexAttribPointer(locations.normal,
                              accessor_byte_size(normal_accessor.type),
                              static_cast<GLenum>(normal_accessor.componentType),
                              normal_accessor.normalized ? GL_TRUE : GL_FALSE,
                              normal_buffer_view.byteStride,
                              reinterpret_cast<void *>(normal_accessor.byteOffset)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast,
                                                                                     // performance-no-int-to-ptr)
    }

    GLuint uvVbo{};
    {
        glGenBuffers(1, &uvVbo);
        glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
        glBufferData(GL_ARRAY_BUFFER,
                     uv_buffer_view.byteLength,
                     uv_buffer.data.data() + uv_buffer_view.byteOffset,
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(locations.uv);
        glVertexAttribPointer(locations.uv,
                              accessor_byte_size(uv_accessor.type),
                              static_cast<GLenum>(uv_accessor.componentType),
                              uv_accessor.normalized ? GL_TRUE : GL_FALSE,
                              uv_buffer_view.byteStride,
                              reinterpret_cast<void *>(uv_accessor.byteOffset)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast,
                                                                                 // performance-no-int-to-ptr)
    }

    GLuint tangentVbo{};
    {
        glGenBuffers(1, &tangentVbo);
        glBindBuffer(GL_ARRAY_BUFFER, tangentVbo);
        glBufferData(GL_ARRAY_BUFFER,
                     tangent_buffer_view.byteLength,
                     tangent_buffer.data.data() + tangent_buffer_view.byteOffset,
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(locations.tangent);
        glVertexAttribPointer(locations.tangent,
                              accessor_byte_size(tangent_accessor.type),
                              static_cast<GLenum>(tangent_accessor.componentType),
                              tangent_accessor.normalized ? GL_TRUE : GL_FALSE,
                              tangent_buffer_view.byteStride,
                              reinterpret_cast<void *>(tangent_accessor.byteOffset)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast,
                                                                                      // performance-no-int-to-ptr)
    }

    GLuint ebo{};
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices_buffer_view.byteLength,
                 indices_buffer.data.data() + indices_buffer_view.byteOffset,
                 GL_STATIC_DRAW);

    glBindVertexArray(0);

    m_vao = vao;
    m_ebo = ebo;
    m_positionVbo = positionVbo;
    m_normalVbo = normalVbo;
    m_uvVbo = uvVbo;
    m_tangentVbo = tangentVbo;
    m_indicesCount = indices_accessor.count;
    m_indicesType = indices_accessor.componentType;
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_vao);

    glDeleteBuffers(1, &m_positionVbo);
    glDeleteBuffers(1, &m_normalVbo);
    glDeleteBuffers(1, &m_uvVbo);
    glDeleteBuffers(1, &m_tangentVbo);
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
