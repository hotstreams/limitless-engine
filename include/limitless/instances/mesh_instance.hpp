#pragma once

#include <limitless/instances/material_instance.hpp>
#include <limitless/models/abstract_mesh.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/core/uniform_setter.hpp>

namespace Limitless {
    class Assets;
    enum class ShaderPass;
    enum class ModelShader;

    class MeshInstance final {
    private:
        std::shared_ptr<AbstractMesh> mesh;
        MaterialInstance material;
        bool hidden {};
    public:
        MeshInstance(std::shared_ptr<AbstractMesh> mesh, const std::shared_ptr<ms::Material>& material) noexcept;
        ~MeshInstance() = default;

        MeshInstance(const MeshInstance&) = default;
        MeshInstance(MeshInstance&&) = default;

        [[nodiscard]] const auto& getMaterial() const noexcept { return material; }
        [[nodiscard]] auto& getMaterial() noexcept { return material; }
        [[nodiscard]] bool isHidden() const noexcept { return hidden; }

        void hide() noexcept;
        void reveal() noexcept;

        void draw(Context& ctx,
                  const Assets& assets,
                  ShaderPass pass,
                  ModelShader model,
                  const glm::mat4& model_matrix,
                  ms::Blending blending,
                  const UniformSetter& uniform_setter);
    };
}