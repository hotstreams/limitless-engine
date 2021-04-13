#pragma once

#include <limitless/models/abstract_mesh.hpp>
#include <limitless/core/vertex_array.hpp>
#include <limitless/core/buffer_builder.hpp>

namespace LimitlessEngine {
    enum class MeshDataType {
        Static,
        Dynamic,
        Stream
    };

    enum class DrawMode {
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleStripAdj = GL_TRIANGLE_STRIP_ADJACENCY,
        TriangleFan = GL_TRIANGLE_FAN,
        Lines = GL_LINES,
        LineLoop = GL_LINE_LOOP,
        Points = GL_POINTS
    };

    template<typename T>
    class Mesh : public AbstractMesh {
    protected:
        std::unique_ptr<Buffer> vertex_buffer;
        VertexArray vertex_array;
        std::vector<T> vertices;

        MeshDataType data_type;
        DrawMode draw_mode;
        std::string name;

        BoundingBox bounding_box;
    private:
        void initialize(size_t count) {
            BufferBuilder builder;
            builder .setTarget(Buffer::Type::Array)
                    .setData(vertices.empty() ? nullptr : vertices.data())
                    .setDataSize(count * sizeof(T));

            switch (data_type) {
                case MeshDataType::Static:
                    vertex_buffer = builder .setUsage(Buffer::Storage::Static)
                                            .setAccess(Buffer::ImmutableAccess::None)
                                            .build();
                    break;
                case MeshDataType::Dynamic:
                    vertex_buffer = builder .setUsage(Buffer::Usage::DynamicDraw)
                                            .setAccess(Buffer::MutableAccess::WriteOrphaning)
                                            .build();
                    break;
                case MeshDataType::Stream:
//                    vertex_buffer = builder .setUsage(Buffer::Storage::DynamicCoherentWrite)
//                                            .setAccess(Buffer::ImmutableAccess::WriteCoherent)
//                                            .buildTriple();
//TODO:
                    break;
            }

            vertex_array << std::pair<T, Buffer&>(T{}, *vertex_buffer);
        }

        void calculateBoundingBox() {
            bounding_box = LimitlessEngine::calculateBoundingBox(vertices);
        }
    public:
        Mesh(std::vector<T>&& _vertices, std::string _name, MeshDataType _data_type, DrawMode _draw_mode)
            : vertices{std::move(_vertices)}
            , data_type{_data_type}
            , draw_mode{_draw_mode}
            , name{std::move(_name)} {

            initialize(vertices.size());
            calculateBoundingBox();
        }

        Mesh(size_t count, std::string _name, MeshDataType _data_type, DrawMode _draw_mode)
            : vertices{}
            , data_type{_data_type}
            , draw_mode{_draw_mode}
            , name{std::move(_name)} {

            initialize(count);
            calculateBoundingBox();
        }

        ~Mesh() override = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        void draw() const noexcept override {
            if (vertices.empty()) {
                return;
            }

            vertex_array.bind();

            glDrawArrays(static_cast<GLenum>(draw_mode), 0, vertices.size());

            vertex_buffer->fence();
        }

        void draw_instanced(size_t count) const noexcept override {
            if (vertices.empty() || !count) {
                return;
            }

            vertex_array.bind();

            glDrawArraysInstanced(static_cast<GLenum>(draw_mode), 0, vertices.size(), count);

            vertex_buffer->fence();
        }

        template<typename Vertices>
        void updateVertices(Vertices&& new_vertices) {
            vertices = std::forward<Vertices>(new_vertices);

            if (vertices.size() * sizeof(T) > vertex_buffer->getSize()) {
                initialize(vertices.size());
                //todo resize
            }

            vertex_buffer->mapData(vertices.data(), sizeof(T) * vertices.size());
        }

        [[nodiscard]] const BoundingBox& getBoundingBox() noexcept override { return bounding_box; }
        [[nodiscard]] const std::string& getName() const noexcept override { return name; }
        [[nodiscard]] const auto& getVertices() const noexcept { return vertices; }
    };
}