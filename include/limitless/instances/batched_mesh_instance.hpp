#pragma once

#include <limitless/ms/batched_material.hpp>
#include <limitless/models/batched_mesh.hpp>

namespace Limitless {
    class BatchedMeshInstance {
    private:
        std::shared_ptr<ms::BatchedMaterial> batched_material;
        std::shared_ptr<BatchedMesh> batched_mesh;
    public:

    };
}