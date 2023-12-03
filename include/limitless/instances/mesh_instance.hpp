#pragma once

#include <limitless/models/abstract_mesh.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>
#include <limitless/core/context.hpp>
#include <memory>

namespace Limitless {
    class Assets;
    enum class ShaderType;
    enum class InstanceType;
}

namespace Limitless {
    class MeshInstance final {
    private:
        std::shared_ptr<AbstractMesh> mesh;
        std::shared_ptr<ms::Material> material;
        std::shared_ptr<ms::Material> base;
    public:
        MeshInstance(std::shared_ptr<AbstractMesh> mesh, const std::shared_ptr<ms::Material>& material) noexcept;
        ~MeshInstance() = default;

        MeshInstance(const MeshInstance&);
        MeshInstance(MeshInstance&&) noexcept = default;

        // changes current material
        void changeMaterial(const std::shared_ptr<ms::Material>& material) noexcept;

        // changes base material
        void changeBaseMaterial(const std::shared_ptr<ms::Material>& material) noexcept;

        // resets base material to base
        void reset() noexcept;

        [[nodiscard]] const auto& getMaterial() const noexcept { return material; }
        [[nodiscard]] auto& getMaterial() noexcept { return material; }

        void update();

        void draw(Context& ctx,
                  const Assets& assets,
                  ShaderType pass,
                  InstanceType model,
                  const glm::mat4& model_matrix,
                  ms::Blending blending,
                  const UniformSetter& uniform_setter);

        void draw_instanced(Context& ctx,
                            const Assets& assets,
                            ShaderType pass,
                            InstanceType model,
                            const glm::mat4& model_matrix,
                            ms::Blending blending,
                            const UniformSetter& uniform_setter,
                            uint32_t count);
    };
}