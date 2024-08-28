#pragma once

#include <limitless/models/abstract_mesh.hpp>
#include <limitless/core/vertex_stream.hpp>
#include <limitless/core/abstract_vertex_stream.hpp>

namespace Limitless {
    class Mesh : public AbstractMesh {
    private:
        std::unique_ptr<AbstractVertexStream> stream;
        std::string name;
        Box bounding_box {};

        void calculateBoundingBox() {
            //TODO: dispatch?
            if (auto vnt = dynamic_cast<VertexStream<VertexNormalTangent>*>(stream.get()); vnt) {
                bounding_box = Limitless::calculateBoundingBox(vnt->getVertices());
            }
            if (auto vnt = dynamic_cast<VertexStream<VertexTerrain>*>(stream.get()); vnt) {
                bounding_box = Limitless::calculateBoundingBox(vnt->getVertices());
            }
        }
    public:
//        Mesh(std::vector<Vertex>&& vertices, VertexStreamUsage usage, VertexStreamDraw draw, std::string _name)
//            : stream {std::move(vertices), usage, draw}
//            , name {std::move(_name)} {
//            calculateBoundingBox();
//        }

//        Mesh(size_t count, VertexStreamUsage usage, VertexStreamDraw draw, std::string _name)
//            : stream {count, usage, draw}
//            , name {std::move(_name)} {
//            calculateBoundingBox();
//        }

        explicit Mesh(std::unique_ptr<AbstractVertexStream> _stream, std::string _name)
            : stream {std::move(_stream)}
            , name {std::move(_name)} {
            calculateBoundingBox();
        }

        ~Mesh() override = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        [[nodiscard]] const Box& getBoundingBox() noexcept override { return bounding_box; }
        [[nodiscard]] const std::string& getName() const noexcept override { return name; }
        [[nodiscard]] std::string& getName() noexcept override { return name; }

        auto& getVertexStream() noexcept { return *stream; }
        [[nodiscard]] const auto& getVertexStream() const noexcept { return *stream; }

        void draw() noexcept override {
            stream->draw();
        }

        void draw(VertexStreamDraw draw) noexcept override {
            stream->draw(draw);
        }

        void draw_instanced(std::size_t count) noexcept override {
            stream->draw_instanced(count);
        }

        void draw_instanced(VertexStreamDraw draw, std::size_t count) noexcept override {
            stream->draw_instanced(draw, count);
        }
    };
}
