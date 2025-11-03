#pragma once

#include <limitless/models/mesh.hpp>

namespace Limitless {
    class BatchedMesh : public Mesh {
    public:
        struct Description {
            std::string name;
            size_t indices_count;
            size_t indices_offset;
//            std::shared_ptr<Material> mat;

            Description(std::string name, size_t indices_count, size_t indices_offset)
                : name(std::move(name))
                , indices_count(indices_count)
                , indices_offset(indices_offset) {
            }
        };
    private:
        std::unordered_map<std::string, Description> descriptions;
    public:
        BatchedMesh(std::shared_ptr<VertexStream> stream, std::string name, decltype(descriptions) descriptions)
            : Mesh()
            , descriptions(std::move(descriptions)) {
            this->name = std::move(name);
            this->stream = std::move(stream);
            // Initialize with default LOD data
            // this->lods = {{0, 0, nullptr, Box{glm::vec3(0.0f), glm::vec3(1.0f)}}};
            // this->transition = LodTransition::None;
            // this->selection = LodSelection::CameraDistance;
            // this->distances = {0.0f};
            calculateBoundingBox();
        }
    };
}