#pragma once

#include <limitless/renderer/shader_type.hpp>

#include <limitless/instances/instanced_instance.hpp>
#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/instances/terrain_instance.hpp>
#include <limitless/fx/effect_renderer.hpp>
#include <limitless/util/frustum_culling.hpp>

namespace Limitless {
    class DrawParameters {
    public:
        Context& ctx;
        const Assets& assets;
        ShaderType type;
        ms::Blending blending;
        UniformSetter setter {};
        UniformInstanceSetter isetter {};
    };

    class InstanceRenderer {
    private:
        FrustumCulling frustum_culling;
        fx::EffectRenderer effect_renderer;

        /**
         * Sets shader and context state according to parameters
         */
        static void setRenderState(const Instance& instance, const MeshInstance& mesh, const DrawParameters& drawp);

        /**
         * Checks whether instance should be rendered for specified parameters
         */
        static bool shouldBeRendered(const Instance& instance, const DrawParameters& drawp);

        /**
         * Renders only visible subset of InstancedInstance instances from frustum culling
         */
        void renderVisibleInstancedInstance(InstancedInstance& instance, const DrawParameters& drawp);
        /**
         * Renders only visible MeshInstances of terrain
         */
        void renderVisibleTerrain(TerrainInstance& instance, const DrawParameters& drawp);
        void renderVisible(Instance& instance, const DrawParameters& drawp);

    public:
        void update(Scene& scene, Camera& camera);

        /**
         * Renders instances from prepared scene in [update] method
         *
         * DecalInstances are not rendered here because they can ONLY use ShaderType::Decal
         */
        void renderScene(const DrawParameters& drawp);

        /**
         * Renders decal instances from prepared scene in [update] method
         */
        void renderDecals(const DrawParameters& drawp);

        /**
         * Static methods to 'just' render instances as it is
         *
         * Does NOT render EffectInstance! use EffectRenderer instead
         *
         * When rendering InstancedInstance beware of 'setVisibleInstances': it should be set to all instances
         */
        static void render(Instance& instance, const DrawParameters& drawp);
        static void render(ModelInstance& instance, const DrawParameters& drawp);
        static void render(SkeletalInstance& instance, const DrawParameters& drawp);
        static void render(InstancedInstance& instance, const DrawParameters& drawp);
        static void render(TerrainInstance& instance, const DrawParameters& drawp);
        static void render(DecalInstance& instance, const DrawParameters& drawp);

        [[nodiscard]] const FrustumCulling& getFrustumCulling() const noexcept { return frustum_culling; }
    };
}