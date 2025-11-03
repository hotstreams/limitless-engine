#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/instances/batched_mesh_instance.hpp>

namespace Limitless {
    class BatchedModelInstance : public ModelInstance {
    private:
        BatchedMeshInstance batched_meshes;

        // info for rendering meshes
    public:
        void update(const Limitless::Camera &camera) override {
            // we should determine whether we can batch the materials
            // because their properties can be changed at runtime


        }
    };
}