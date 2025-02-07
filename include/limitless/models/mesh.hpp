#pragma once

#include <limitless/util/box.hpp>
#include <limitless/core/vertex_stream/vertex_stream.hpp>

namespace Limitless {
    class Mesh {
    protected:
        std::shared_ptr<VertexStream> stream;
        std::string name;
        Box bounding_box {};

        void calculateBoundingBox() {
            //TODO: dispatch?
//            if (auto vnt = dynamic_cast<VertexStream<VertexNormalTangent>*>(stream.get()); vnt) {
//                bounding_box = Limitless::calculateBoundingBox(vnt->getVertices());
//            }
//            if (auto vnt = dynamic_cast<VertexStream<VertexTerrain>*>(stream.get()); vnt) {
//                bounding_box = Limitless::calculateBoundingBox(vnt->getVertices());
//            }
            bounding_box = Box{glm::vec3(0.0f), glm::vec3(555.0f)};
        }

        explicit Mesh(std::shared_ptr<VertexStream> _stream, std::string _name)
            : stream {std::move(_stream)}
            , name {std::move(_name)} {
                calculateBoundingBox();
            }
    public:
        virtual ~Mesh() = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        [[nodiscard]] const Box& getBoundingBox() noexcept { return bounding_box; }
        [[nodiscard]] const std::string& getName() const noexcept { return name; }
        [[nodiscard]] std::string& getName() noexcept { return name; }

        auto& getVertexStream() noexcept { return *stream; }
        [[nodiscard]] const auto& getVertexStream() const noexcept { return *stream; }

        void draw() noexcept {
            stream->draw();
        }

        void draw_instanced(std::size_t count) noexcept {
            stream->draw_instanced(count);
        }

        class Builder;
        static Builder builder();
    };
}
