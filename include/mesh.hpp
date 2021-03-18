#pragma once

#include <core/vertex_array.hpp>
#include <core/buffer_builder.hpp>

namespace LimitlessEngine {
    class AbstractMesh {
    public:
        AbstractMesh() = default;
        virtual ~AbstractMesh() = default;

        virtual void draw() const noexcept = 0;
        virtual void draw_instanced(size_t count) const noexcept = 0;

        [[nodiscard]] virtual const std::string& getName() const noexcept = 0;
    };

    enum class MeshDataType { Static, Dynamic, Stream };
    enum class DrawMode {
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleStripAdj = GL_TRIANGLE_STRIP_ADJACENCY,
        TriangleFan = GL_TRIANGLE_FAN,
        Lines = GL_LINES,
        LineLoop = GL_LINE_LOOP
    };

    template<typename T>
    class Mesh : public AbstractMesh {
    protected:
        std::vector<T> vertices;
        std::string name;

        MeshDataType data_type;
        DrawMode draw_mode;

        VertexArray vertex_array;
        std::unique_ptr<Buffer> vertex_buffer;
    private:
        void initialize() {
            switch (data_type) {
                case MeshDataType::Static:
                    vertex_buffer = BufferBuilder::build(Buffer::Type::Array, vertices, Buffer::Storage::Static, Buffer::ImmutableAccess::None);
                    break;
                case MeshDataType::Dynamic:
                    vertex_buffer = BufferBuilder::build(Buffer::Type::Array, vertices, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
                    break;
                case MeshDataType::Stream:
                    vertex_buffer = BufferBuilder::buildTriple(Buffer::Type::Array, vertices, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);
                    break;
            }

            vertex_array << std::pair<T, Buffer&>(T{}, *vertex_buffer);
        }
    public:
        Mesh(std::vector<T>&& vertices, std::string name, MeshDataType data_type, DrawMode draw_mode)
            : vertices{std::move(vertices)}, name{std::move(name)}, data_type{data_type}, draw_mode{draw_mode} {
            initialize();
        }

        Mesh(std::string name, MeshDataType _data_type, DrawMode _draw_mode)
            : vertices{1000}, name{std::move(name)}, data_type{_data_type}, draw_mode{_draw_mode} {
            initialize();
        }

        ~Mesh() override = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        void draw() const noexcept override {
            if (vertices.empty()) return;

            vertex_array.bind();

            glDrawArrays(static_cast<GLenum>(draw_mode), 0, vertices.size());

            vertex_buffer->fence();
        }

        void draw_instanced(size_t count) const noexcept override {
            if (vertices.empty() || !count) return;

            vertex_array.bind();

            glDrawArraysInstanced(static_cast<GLenum>(draw_mode), 0, vertices.size(), count);

            vertex_buffer->fence();
        }

        template<typename VertexArray>
        void updateVertices(VertexArray&& new_vertices) {
            vertices = std::forward<VertexArray>(new_vertices);

            if (vertices.size() * sizeof(T) > vertex_buffer->getSize()) {
                initialize();
            }

            vertex_buffer->mapData(vertices.data(), sizeof(T) * vertices.size());
        }

        [[nodiscard]] const std::string& getName() const noexcept override { return name; }
        [[nodiscard]] const auto& getVertices() const noexcept { return vertices; }
    };
}