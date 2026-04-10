#pragma once

#include <limitless/renderer/shader_type.hpp>
#include <limitless/renderer/render_queue.hpp>

#include <limitless/instances/instanced_instance.hpp>
#include <limitless/instances/skeletal_instanced_instance.hpp>
#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/instances/terrain_instance.hpp>
#include <limitless/fx/effect_renderer.hpp>
#include <limitless/util/frustum_culling.hpp>

namespace Limitless {
    class RendererSettings;

    class DrawParameters {
    public:
        Context& ctx;
        const Assets& assets;
        ShaderType type;
        ms::Blending blending;
        UniformSetter setter {};
        UniformInstanceSetter isetter {};
    };
    inline uint32_t instance_index = 0;
    class InstanceRenderer {
    private:
        FrustumCulling frustum_culling;
        fx::EffectRenderer effect_renderer;
        RenderQueue render_queue;

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
        void renderVisibleSkeletalInstancedInstance(SkeletalInstancedInstance& instance, const DrawParameters& drawp);
        /**
         * Renders only visible MeshInstances of terrain
         */
        void renderVisibleTerrain(TerrainInstance& instance, const DrawParameters& drawp);
        void renderVisible(Instance& instance, const DrawParameters& drawp);

    public:
        void update(Scene& scene, Camera& camera);

        /**
         * Renders only visible terrain instances (uses internal visibility data)
         */
        void renderVisibleTerrainOnly(const DrawParameters& drawp);

        /**
         * Renders all visible non-terrain, non-decal instances
         */
        void renderVisibleNonTerrain(const DrawParameters& drawp);

        /**
         * Renders instances from prepared scene in [update] method
         *
         * DecalInstances are not rendered here because they can ONLY use ShaderType::Decal
         */
        void renderScene(const DrawParameters& drawp);

        /**
         * Render from explicit instance lists (used by shadow culling / special passes).
         *
         * - instances: top-level instances to render (may include Instanced/Terrain containers)
         * - visible_subinstances: optional map for InstancedInstance / Terrain sub-instances
         *   (same semantics as FrustumCulling internal map).
         */
        void renderSceneFromLists(
                const DrawParameters& drawp,
                const Instances& instances,
                const std::unordered_map<uint64_t, std::vector<std::shared_ptr<ModelInstance>>>& visible_subinstances);

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
        static void render(SkeletalInstancedInstance& instance, const DrawParameters& drawp);
        static void render(TerrainInstance& instance, const DrawParameters& drawp);
        static void render(DecalInstance& instance, const DrawParameters& drawp);

        [[nodiscard]] const FrustumCulling& getFrustumCulling() const noexcept { return frustum_culling; }
        [[nodiscard]] const RenderQueue& getRenderQueue() const noexcept { return render_queue; }

        /**
         * Prepares RenderQueue for sorted rendering (must be called before renderSceneSorted)
         */
        void prepareSortedRendering(const Camera& camera, const RendererSettings& settings);

        /**
         * Renders scene using sorted RenderQueue for optimal state changes
         *
         * Uses material/shader sorting for opaque, back-to-front for translucent
         */
        void renderSceneSorted(const DrawParameters& drawp);

        /**
         * Renders visible terrain only using sorted RenderQueue
         */
        void renderVisibleTerrainOnlySorted(const DrawParameters& drawp);

        /**
         * Renders visible non-terrain instances using sorted RenderQueue
         */
        void renderVisibleNonTerrainSorted(const DrawParameters& drawp);

        /**
         * Renders only visible instances NOT handled by indirect draw.
         * Skips Model/Instanced instances whose meshes are all batched (have draw_info).
         * Also skips terrain and decals.
         */
        void renderVisibleNonBatchedNonTerrain(const DrawParameters& drawp);

        /**
         * Sorted version: renders non-batched non-terrain instances via RenderQueue
         */
        void renderVisibleNonBatchedNonTerrainSorted(const DrawParameters& drawp);
    };
}