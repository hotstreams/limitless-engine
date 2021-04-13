#pragma once

#include <limitless/util/bounding_box.hpp>
#include <memory>

namespace LimitlessEngine {
    class AbstractMesh;

    class AbstractModel {
    protected:
        std::vector<std::shared_ptr<AbstractMesh>> meshes;
        BoundingBox bounding_box {};

        void calculateBoundingBox();
    public:
        explicit AbstractModel(decltype(meshes)&& _meshes);
        virtual ~AbstractModel() = default;

        [[nodiscard]] const auto& getMeshes() const noexcept { return meshes; };
        [[nodiscard]] const auto& getBoundingBox() const noexcept { return bounding_box; }
    };
}