#pragma once

#include <cstdint>
#include <limitless/util/box.hpp>
#include <limitless/core/vertex_stream/vertex_stream.hpp>
#include <limitless/core/vertex_stream/indexed_stream.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/core/indirect/indirect_draw_command.hpp>
#include <optional>

namespace Limitless {
    class Mesh {
    protected:
        std::string name;

        std::shared_ptr<VertexStream> stream;

        Box bounding_box;

        /**
         * Draw info for indirect rendering (batched geometry)
         *
         * Contains offset and count within the shared BatchedVertexStream.
         * Only set when using indirect draw mode.
         */
        std::optional<MeshDrawInfo> draw_info;

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

        /**
         * Get draw info for indirect rendering
         *
         * @return Pointer to MeshDrawInfo if using batched geometry, nullptr otherwise
         */
        [[nodiscard]] const MeshDrawInfo* getDrawInfo() const noexcept {
            return draw_info.has_value() ? &draw_info.value() : nullptr;
        }

        /**
         * Set draw info for indirect rendering (used by builder when batching)
         */
        void setDrawInfo(const MeshDrawInfo& info) noexcept {
            draw_info = info;
        }

        /**
         * Check if mesh supports indirect draw
         */
        [[nodiscard]] bool supportsIndirectDraw() const noexcept {
            return draw_info.has_value();
        }

        void draw() noexcept {
            if (draw_info && stream) {
                if (auto* indexed = dynamic_cast<IndexedStream*>(stream.get())) {
                    indexed->drawBatched(*draw_info);
                    return;
                }
            }
            stream->draw();
        }

        void draw_instanced(std::size_t count) noexcept {
            draw_instanced(count, 0);
        }

        void draw_instanced(std::size_t count, std::uint32_t base_instance) noexcept {
            if (draw_info && stream) {
                if (auto* indexed = dynamic_cast<IndexedStream*>(stream.get())) {
                    indexed->drawBatchedInstanced(*draw_info, count, base_instance);
                    return;
                }
            }
            stream->draw_instanced(count, base_instance);
        }

        class Builder;
        static Builder builder();
    };
}
