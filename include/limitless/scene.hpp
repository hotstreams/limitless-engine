#pragma once

#include <limitless/lighting/lighting.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/instances/instance_builder.hpp>
#include <limitless/skybox/skybox.hpp>
#include <limitless/camera.hpp>
#include <stdexcept>
#include <unordered_map>
#include <memory>

namespace Limitless {
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
        Lighting lighting;
        std::unordered_map<uint64_t, std::shared_ptr<Instance>> instances;
        std::shared_ptr<Skybox> skybox;
        float animation_playback_speed {1.f};
        void removeDeadInstances() noexcept;
        void applyAnimationPlaybackSpeed(Instance& instance) const;
    public:
        explicit Scene(Context& context);

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

        const std::shared_ptr<Skybox>& getSkybox() const noexcept;
        std::shared_ptr<Skybox>& getSkybox() noexcept;
        void setSkybox(const std::shared_ptr<Skybox>& skybox);

        /**
         * Return visible scene instances.
         */
        Instances getInstances() const noexcept;

        /**
         * Multiplier applied to skeletal clip playback and particle emitters
         * (1 = real time, 0 = frozen). Applied to existing and newly added
         * skeletal and effect instances on the next update.
         */
        void setAnimationPlaybackSpeed(float speed) noexcept;
        [[nodiscard]] auto getAnimationPlaybackSpeed() const noexcept { return animation_playback_speed; }

        void update(const Camera& camera);
    };
}