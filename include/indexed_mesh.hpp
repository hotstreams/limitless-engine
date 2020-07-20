#pragma once

#include <mesh.hpp>

namespace GraphicsEngine {
    template<typename T, typename T1>
    class IndexedMesh : public Mesh<T> {
    protected:
        std::vector<T1> indices;
        std::unique_ptr<Buffer> indices_buffer;
    private:
        void initialize() {
            switch (this->data_type) {
                case MeshDataType::Static:
                    indices_buffer = BufferBuilder::build(Buffer::Type::Element, indices, Buffer::Storage::Static, Buffer::ImmutableAccess::None);
                    break;
                case MeshDataType::Dynamic:
                    indices_buffer = BufferBuilder::build(Buffer::Type::Element, indices, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
                    break;
                case MeshDataType::Stream:
                    indices_buffer = BufferBuilder::buildTriple(Buffer::Type::Element, indices, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);
                    break;
            }

            this->vertex_array << *indices_buffer;
        }

        [[nodiscard]] constexpr GLenum getIndicesType() const noexcept {
            if constexpr (std::is_same<T1, GLuint>::value) {
                return GL_UNSIGNED_INT;
            } else if constexpr (std::is_same<T1, GLushort>::value) {
                return GL_UNSIGNED_SHORT;
            } else if constexpr (std::is_same<T1, GLubyte>::value) {
                return GL_UNSIGNED_BYTE;
            } else {
                static_assert(!std::is_same<T1, T1>::value, "Wrong type for indices");
            }
        }
    public:
        IndexedMesh(std::vector<T>&& vertices, std::vector<T1>&& indices, std::string material, MeshDataType data_type, DrawMode draw_mode)
            : Mesh<T>{std::move(vertices), std::move(material), data_type, draw_mode}, indices{std::move(indices)} {
            initialize();
        }

        ~IndexedMesh() override = default;

        IndexedMesh(const IndexedMesh&) noexcept = delete;
        IndexedMesh& operator=(const IndexedMesh&) noexcept = delete;

        IndexedMesh(IndexedMesh&&) noexcept = default;
        IndexedMesh& operator=(IndexedMesh&&) noexcept = default;

        void draw() const noexcept override {
            this->vertex_array.bind();

            glDrawElements(static_cast<GLenum>(this->draw_mode), indices.size(), getIndicesType(), nullptr);
        }
    };
}