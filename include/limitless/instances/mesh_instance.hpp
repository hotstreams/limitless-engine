#pragma once

#include <limitless/models/mesh.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>
#include <limitless/core/context.hpp>
#include <limitless/camera.hpp>
#include <memory>

namespace Limitless {
    class Assets;
    enum class ShaderType;
    enum class InstanceType;
}

namespace Limitless {
    class MeshInstance final {
    private:
        std::shared_ptr<Mesh> mesh;

        struct LodMaterial
        {
            std::shared_ptr<ms::Material> material;
            std::shared_ptr<ms::Material> base;

            LodMaterial(const ms::Material& mat)
                : material(std::make_shared<ms::Material>(mat))
                , base(std::make_shared<ms::Material>(mat))
            {

            }
        };
        std::unordered_map<uint32_t, LodMaterial> lods;

        uint32_t current_lod;

        void selectLod(const Camera& camera, const glm::vec3& position);
    public:
        MeshInstance(std::shared_ptr<Mesh> mesh) noexcept;
        ~MeshInstance() = default;

        MeshInstance(const MeshInstance&);
        MeshInstance(MeshInstance&&) noexcept = default;    

        // changes current material for current lod
        void changeMaterial(const std::shared_ptr<ms::Material>& material) noexcept;

        // changes base material for current lod
        void changeBaseMaterial(const std::shared_ptr<ms::Material>& material) noexcept;

        // resets base material to base material for current lod
        void reset() noexcept;

        [[nodiscard]] const auto& getMaterial() const noexcept { return lods.at(current_lod).material; }
        [[nodiscard]] auto& getMaterial() noexcept { return lods.at(current_lod).material; }

        [[nodiscard]] const std::shared_ptr<Mesh>& getMesh() const noexcept { return mesh; }
        [[nodiscard]] const uint32_t& getCurrentLod() const noexcept { return current_lod; }

        void update(const Camera& camera, const glm::vec3& position);
    };
}