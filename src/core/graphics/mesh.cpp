#include "mesh.h"

#include <exception>

GpuMesh::GpuMesh(Mesh const& mesh)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vertex attributes
    for (auto const& [attribute_id, attribute_data] : mesh.attributes) {
        // BUG: https://github.com/llvm/llvm-project/issues/48582
        auto attr_id = attribute_id;

        GLuint vbo{};
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        std::visit(
            [attr_id](auto&& arg) {
                glBufferData(GL_ARRAY_BUFFER, arg.size(), arg.data(), GL_STATIC_DRAW);

                int const components = [arg]() -> int {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, VertexAttributeData::Scalar>) {
                        return 1;
                    }
                    if constexpr (std::is_same_v<T, VertexAttributeData::Vec2>) {
                        return 2;
                    }
                    if constexpr (std::is_same_v<T, VertexAttributeData::Vec3>) {
                        return 3;
                    }
                    if constexpr (std::is_same_v<T, VertexAttributeData::Vec4>) {
                        return 4;
                    }

                    std::terminate();
                }();

                glEnableVertexAttribArray(attr_id);
                glVertexAttribPointer(
                    attr_id,
                    components,
                    GL_FLOAT, // static_cast<GLenum>(position_accessor.componentType),
                    false,    // position_accessor.normalized ? GL_TRUE : GL_FALSE,
                    0,
                    nullptr);
            },
            attribute_data.values);
    }

    // Indices
    GLuint ebo{};
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    std::visit(
        [this](auto&& arg) {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, arg.size(), arg.data(), GL_STATIC_DRAW);

            indices_count = arg.size();
            indices_type = [arg]() -> GLenum {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Indices::UnsignedByte>) {
                    return GL_UNSIGNED_BYTE;
                }
                if constexpr (std::is_same_v<T, Indices::UnsignedShort>) {
                    return GL_UNSIGNED_SHORT;
                }
                if constexpr (std::is_same_v<T, Indices::UnsignedInt>) {
                    return GL_UNSIGNED_INT;
                }

                std::terminate();
            }();
        },
        mesh.indices.values);

    glBindVertexArray(0);
}
