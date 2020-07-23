#pragma once

#include <mesh.hpp>
#include <material_instance.hpp>

namespace GraphicsEngine {
    class MeshInstance {
    private:
        std::shared_ptr<AbstractMesh> mesh;
        MaterialInstance material;
        bool hidden;
    public:
        MeshInstance(std::shared_ptr<AbstractMesh> mesh, const std::shared_ptr<Material>& material);

        [[nodiscard]] auto& getMaterial() const noexcept { return material; }
        [[nodiscard]] bool isHidden() const noexcept { return hidden; }
        void hide() noexcept;
        void reveal() noexcept;

        void draw() const noexcept;
    };
}