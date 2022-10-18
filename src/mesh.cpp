#include "mesh.h"

GpuMesh::GpuMesh(Mesh const &mesh)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    for (auto const &[attribute_id, attribute_data] : mesh.attributes) {
        // BUG: https://github.com/llvm/llvm-project/issues/48582
        auto attr_id = attribute_id;

        std::visit([=](auto&& arg) {
            GLuint vbo{};
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, arg.size(), arg.data(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(attr_id);
            glVertexAttribPointer(
                attr_id,
                accessor_byte_size(position_accessor.type),
                static_cast<GLenum>(position_accessor.componentType),
                position_accessor.normalized ? GL_TRUE : GL_FALSE,
                position_buffer_view.byteStride,
                reinterpret_cast<void *>(
                    position_accessor
                        .byteOffset)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast,
                                       // performance-no-int-to-ptr)
    }
    }
