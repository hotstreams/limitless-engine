#pragma once

#include <limitless/lighting/lighting.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/instances/instance_builder.hpp>
#include <limitless/skybox/skybox.hpp>
#include <limitless/wind/speedtree_wind_state.hpp>
#include <limitless/camera.hpp>
#include <stdexcept>
#include <unordered_map>
#include <memory>

namespace Limitless {
    class WindController;

    class scene_exception : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    using Instances = std::vector<std::shared_ptr<Instance>>;

    /**
     *
     */
    class Scene final {
    private:
        struct Wind {
            bool enabled {false};
            // World-space direction (should be normalized by user).
            glm::vec3 direction {1.0f, 0.0f, 0.0f};
            // Global strength multiplier.
            float strength {0.0f};
            // Animation speed multiplier.
            float speed {1.0f};
            // Spatial scale for gust/noise.
            float scale {0.1f};
            // Wind quality tier:
            // 0 - None, 1 - Fastest, 2 - Fast, 3 - Good, 4 - Best
            uint32_t quality {0};
            // Time accumulator (seconds). User drives it (e.g. += delta).
            float time {0.0f};
        } wind;

        // Legacy SpeedTree-style wind preset parameters (matches UE4 FSpeedTreeData layout).
        // Still supported as a simple override path (but most users should prefer the CPU wind state).
        struct SpeedTreeWind {
            glm::vec4 wind_vector {0.0f};            // _ST_WindVector
            glm::vec4 wind_global {0.0f};            // _ST_WindGlobal
            glm::vec4 wind_branch {0.0f};            // _ST_WindBranch
            glm::vec4 wind_branch_twitch {0.0f};     // _ST_WindBranchTwitch
            glm::vec4 wind_branch_whip {0.0f};       // _ST_WindBranchWhip
            glm::vec4 wind_branch_anchor {0.0f};     // _ST_WindBranchAnchor
            glm::vec4 wind_branch_adherences {0.0f}; // _ST_WindBranchAdherences
            glm::vec4 wind_turbulences {0.0f};       // _ST_WindTurbulences
            glm::vec4 wind_leaf1_ripple {0.0f};      // _ST_WindLeaf1Ripple
            glm::vec4 wind_leaf1_tumble {0.0f};      // _ST_WindLeaf1Tumble
            glm::vec4 wind_leaf1_twitch {0.0f};      // _ST_WindLeaf1Twitch
            glm::vec4 wind_leaf2_ripple {0.0f};      // _ST_WindLeaf2Ripple
            glm::vec4 wind_leaf2_tumble {0.0f};      // _ST_WindLeaf2Tumble
            glm::vec4 wind_leaf2_twitch {0.0f};      // _ST_WindLeaf2Twitch
            glm::vec4 wind_frond_ripple {0.0f};      // _ST_WindFrondRipple
            glm::vec4 wind_rolling_branch {0.0f};                // WindRollingBranch (UE4)
            glm::vec4 wind_rolling_leaf_and_direction {0.0f};    // WindRollingLeafAndDirection (UE4)
            glm::vec4 wind_rolling_noise {0.0f};                 // WindRollingNoise (UE4)
            glm::vec4 wind_animation {0.0f};         // _ST_WindAnimation

            // Debug mask to isolate components in shader (bitmask):
            // 1  = global
            // 2  = branch1
            // 4  = branch2
            // 8  = leaf ripple
            // 16 = leaf tumble
            uint32_t debug_mask {1u | 2u | 4u | 8u | 16u};
        } speedtree_wind;

        // CPU-side SDK-style wind state (CWind/UE4 FSpeedTreeWind-style).
        // When enabled, SceneData will be populated from this state instead of raw `speedtree_wind` values.
        bool speedtree_use_cpu_state {false};
        SpeedTreeWindState speedtree_state;

        // Central controller for configuring wind/presets (kept out-of-line to avoid header cycles).
        std::unique_ptr<WindController> wind_controller;

        Lighting lighting;
        std::unordered_map<uint64_t, std::shared_ptr<Instance>> instances;
        std::shared_ptr<Skybox> skybox;
        void removeDeadInstances() noexcept;
    public:
        explicit Scene(Context& context);
        ~Scene();

        Scene(const Scene&) = delete;
        Scene(Scene&&) = delete;

        void add(const std::shared_ptr<Instance>& instance);
        void remove(const std::shared_ptr<Instance>& instance);
        void remove(uint64_t id);
        void removeAll();

        std::shared_ptr<Instance> getInstance(uint64_t id);
        std::shared_ptr<ModelInstance> getModelInstance(uint64_t id);
        std::shared_ptr<SkeletalInstance> getSkeletalInstance(uint64_t id);
        std::shared_ptr<EffectInstance> getEffectInstance(uint64_t id);

        const Lighting& getLighting() const noexcept;
        Lighting& getLighting() noexcept;

        Light& add(Light&& light);
        Light& add(const Light& light);

        const Wind& getWind() const noexcept { return wind; }
        Wind& getWind() noexcept { return wind; }

        const SpeedTreeWind& getSpeedTreeWind() const noexcept { return speedtree_wind; }
        SpeedTreeWind& getSpeedTreeWind() noexcept { return speedtree_wind; }

        bool isSpeedTreeWindCpuStateEnabled() const noexcept { return speedtree_use_cpu_state; }
        void setSpeedTreeWindCpuStateEnabled(bool enabled) noexcept { speedtree_use_cpu_state = enabled; }
        const SpeedTreeWindState& getSpeedTreeWindState() const noexcept { return speedtree_state; }
        SpeedTreeWindState& getSpeedTreeWindState() noexcept { return speedtree_state; }

        WindController& getWindController() noexcept { return *wind_controller; }
        const WindController& getWindController() const noexcept { return *wind_controller; }

        const std::shared_ptr<Skybox>& getSkybox() const noexcept;
        std::shared_ptr<Skybox>& getSkybox() noexcept;
        void setSkybox(const std::shared_ptr<Skybox>& skybox);

        /**
         * Return visible scene instances.
         */
        Instances getInstances() const noexcept;

        void update(const Camera& camera);
    };
}