#pragma once

#include <limitless/models/mesh.hpp>
#include <iostream>

namespace Limitless {
    template<typename T, typename T1>
    class IndexedMesh : public Mesh<T> {
    protected:
        std::vector<T1> indices;
        std::unique_ptr<Buffer> indices_buffer;
    private:
        void initialize() {
            BufferBuilder builder;
            builder.setTarget(Buffer::Type::Element)
                    .setData(indices.data())
                    .setDataSize(indices.size() * sizeof(T1));

            switch (this->data_type) {
                case MeshDataType::Static:
                    indices_buffer = builder.setUsage(Buffer::Storage::Static)
                                            .setAccess(Buffer::ImmutableAccess::None)
                                            .build();
                    break;
                case MeshDataType::Dynamic:
                    indices_buffer = builder.setUsage(Buffer::Usage::DynamicDraw)
                                            .setAccess(Buffer::MutableAccess::WriteOrphaning)
                                            .build();
                    break;
                case MeshDataType::Stream:
//                    indices_buffer = builder.setUsage(Buffer::Storage::DynamicCoherentWrite)
//                                            .setAccess(Buffer::ImmutableAccess::WriteCoherent)
//                                            .buildTriple();
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
        IndexedMesh(std::vector<T>&& vertices, std::vector<T1>&& indices, std::string name, MeshDataType data_type, DrawMode draw_mode)
            : Mesh<T>{std::move(vertices), std::move(name), data_type, draw_mode}, indices{std::move(indices)} {
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

            this->vertex_buffer->fence();
            indices_buffer->fence();
        }

        void draw(DrawMode mode) const noexcept override {
            this->vertex_array.bind();

            glDrawElements(static_cast<GLenum>(mode), indices.size(), getIndicesType(), nullptr);

            this->vertex_buffer->fence();
            indices_buffer->fence();
        }

        void draw_instanced(DrawMode mode, size_t count) const noexcept override {
            this->vertex_array.bind();

            glDrawElementsInstanced(static_cast<GLenum>(mode), indices.size(), getIndicesType(), nullptr, count);

            this->vertex_buffer->fence();
            indices_buffer->fence();
        }

        auto& getIndices() noexcept { return indices; }
        const auto& getIndices() const noexcept { return indices; }
    };
}