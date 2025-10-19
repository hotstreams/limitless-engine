#pragma once

#include <limitless/util/box.hpp>
#include <limitless/core/vertex_stream/vertex_stream.hpp>
#include <limitless/ms/material.hpp>

namespace Limitless {
    class Mesh {
    protected:
        std::string name;

        std::shared_ptr<VertexStream> stream;

        Box bounding_box;

        void calculateBoundingBox() {
            bounding_box = Limitless::calculateBoundingBox([&](auto lambda){
                stream->forEach<glm::vec3>(VertexStream::Attribute::Position, lambda);
            });
        }

        Mesh(
            std::string name,
            std::shared_ptr<VertexStream> stream
        )
            : name{std::move(name)}
            , stream{std::move(stream)}
        {
            calculateBoundingBox();
        }

    public:
        virtual ~Mesh() = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        // Default constructor for derived classes
        Mesh() = default;

        [[nodiscard]] const std::string& getName() const noexcept { return name; }
        [[nodiscard]] std::string& getName() noexcept { return name; }
        auto& getVertexStream() noexcept { return *stream; }
        [[nodiscard]] const auto& getVertexStream() const noexcept { return *stream; }
        [[nodiscard]] const Box& getBoundingBox() const noexcept { return bounding_box; }

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
