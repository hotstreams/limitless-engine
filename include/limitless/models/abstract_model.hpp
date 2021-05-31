#pragma once

#include <limitless/util/bounding_box.hpp>
#include <string>
#include <memory>

namespace Limitless {
    class AbstractMesh;

    class AbstractModel {
    protected:
        std::string name;
        std::vector<std::shared_ptr<AbstractMesh>> meshes;
        BoundingBox bounding_box {};

        void calculateBoundingBox();
    public:
        explicit AbstractModel(decltype(meshes)&& _meshes, std::string name);
        virtual ~AbstractModel() = default;

        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] const auto& getMeshes() const noexcept { return meshes; };
        [[nodiscard]] const auto& getBoundingBox() const noexcept { return bounding_box; }
    };
}