#pragma once

#include <limitless/util/box.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Limitless {
    class AbstractMesh;

    class AbstractModel {
    protected:
        std::string name;
        std::filesystem::path source_path;
        std::vector<std::shared_ptr<AbstractMesh>> meshes;
        Box bounding_box {};

        void calculateBoundingBox();
    public:
        explicit AbstractModel(decltype(meshes)&& _meshes, std::string name);
        virtual ~AbstractModel() = default;

        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] const auto& getMeshes() const noexcept { return meshes; };
        [[nodiscard]] const auto& getBoundingBox() const noexcept { return bounding_box; }
        [[nodiscard]] const auto& getSourcePath() const noexcept { return source_path; }
        void setSourcePath(std::filesystem::path path) { source_path = std::move(path); }
    };
}