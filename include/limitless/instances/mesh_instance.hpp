#pragma once

#include <limitless/models/mesh.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>
#include <limitless/core/context.hpp>
#include <limitless/camera.hpp>
#include <memory>

#include "limitless/models/model.hpp"

namespace Limitless {
    class Assets;
    enum class ShaderType;
    enum class InstanceType;
}

namespace Limitless {
    class MeshInstance final {
    private:
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<ms::Material> material;
        std::shared_ptr<ms::Material> base;
    public:
        MeshInstance(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<ms::Material>& material) noexcept;
        ~MeshInstance() = default;

        MeshInstance(const MeshInstance&);
        MeshInstance(MeshInstance&&) noexcept = default;    

        void changeMaterial(const std::shared_ptr<ms::Material>& material) noexcept;
        void changeBaseMaterial(const std::shared_ptr<ms::Material>& material) noexcept;

        // resets base material to base material
        void reset() noexcept;

        [[nodiscard]] const auto& getMaterial() const noexcept { return material; }
        [[nodiscard]] auto& getMaterial() noexcept { return material; }

        [[nodiscard]] const std::shared_ptr<Mesh>& getMesh() const noexcept { return mesh; }

        void update();
    };
}