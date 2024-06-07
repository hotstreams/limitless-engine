#pragma once

#include <limitless/instances/model_instance.hpp>
#include <utility>

namespace Limitless {
    class TerrainInstance : public ModelInstance {
    public:
        TerrainInstance(decltype(model) model, const glm::vec3& position)
            : ModelInstance {InstanceType::Terrain, std::move(model), position} {

        }
    };
}