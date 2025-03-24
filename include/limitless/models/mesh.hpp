#pragma once

#include <limitless/util/box.hpp>
#include <limitless/core/vertex_stream/vertex_stream.hpp>
#include <limitless/ms/material.hpp>

namespace Limitless {
    class Mesh {
    public:
        enum class LodTransition {
            None,
            Fade,
            Speedtree
        };

        enum class LodSelection {
            CameraDistance
        };
    protected:
        std::string name;

        std::shared_ptr<VertexStream> stream;

        struct LodData
        {
            size_t offset;
            size_t count;

            std::shared_ptr<ms::Material> material;

            Box bounding_box;
        };
        std::vector<LodData> lods;

        LodTransition transition;
        
        LodSelection selection;
        std::vector<float> distances;

        void calculateBoundingBox() {
            //TODO: dispatch?
//            if (auto vnt = dynamic_cast<VertexStream<VertexNormalTangent>*>(stream.get()); vnt) {
//                bounding_box = Limitless::calculateBoundingBox(vnt->getVertices());
//            }
//            if (auto vnt = dynamic_cast<VertexStream<VertexTerrain>*>(stream.get()); vnt) {
//                bounding_box = Limitless::calculateBoundingBox(vnt->getVertices());
//            }
            // bounding_box = Box{glm::vec3(0.0f), glm::vec3(555.0f)};
        }

        Mesh(
            std::string name,
            std::shared_ptr<VertexStream> stream,
            std::vector<LodData> lods,
            LodTransition transition,
            LodSelection selection,
            std::vector<float> distances)
            : name{std::move(name)}
            , stream{std::move(stream)}
            , lods{std::move(lods)}
            , transition{transition}
            , selection{selection}
            , distances{std::move(distances)}
        {
            
        }

    public:
        virtual ~Mesh() = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        [[nodiscard]] const std::string& getName() const noexcept { return name; }
        [[nodiscard]] std::string& getName() noexcept { return name; }
        [[nodiscard]] LodSelection getSelection() const noexcept { return selection; }
        [[nodiscard]] LodTransition getTransition() const noexcept { return transition; }   
        [[nodiscard]] const std::vector<float>& getDistances() const noexcept { return distances; }
        [[nodiscard]] const std::vector<LodData>& getLods() const noexcept { return lods; }
        auto& getVertexStream() noexcept { return *stream; }
        [[nodiscard]] const auto& getVertexStream() const noexcept { return *stream; }

        // draws highest lod
        void draw() noexcept {
            const auto& highest = lods[0];
            stream->draw(highest.offset, highest.count);
        }

        void draw_instanced(std::size_t count) noexcept {
            const auto& highest = lods[0];
            stream->draw_instanced(highest.offset, highest.count, count);
        }

        void draw(uint32_t lod) noexcept {
            const auto& lod_data = lods[lod];
            stream->draw(lod_data.offset, lod_data.count);
        }

        void draw_instanced(uint32_t lod, std::size_t count) noexcept {
            const auto& lod_data = lods[lod];
            stream->draw_instanced(lod_data.offset, lod_data.count, count);
        }

        class Builder;
        static Builder builder();
    };
}
