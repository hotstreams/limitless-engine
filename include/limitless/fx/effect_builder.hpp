#pragma once

#include <limitless/fx/emitters/unique_emitter.hpp>
#include <limitless/fx/emitters/emitter_spawn.hpp>
#include <limitless/fx/modules/distribution.hpp>

namespace Limitless {
    class EffectInstance;
    class AbstractMesh;
    class AbstractModel;
    class Context;
    class Assets;
    class EmitterSerializer;

    namespace ms {
        class Material;
    }
}

namespace Limitless::fx {
    class EffectBuilder {
    private:
        std::shared_ptr<EffectInstance> effect;
        std::string effect_name;
        std::string last_emitter;

        Assets& assets;

        template<typename Emitter>
        EffectBuilder& setModules(decltype(Emitter::modules)&& modules);

        EffectBuilder& setSpawn(EmitterSpawn&& spawn);

        template<template<typename P> class M, typename... Args>
        void addModule(Args&&... args);

        template<typename Emitter>
        UniqueEmitterShader getUniqueEmitterShader(const Emitter& emitter) const noexcept;

        friend class Limitless::EmitterSerializer;
    public:
        explicit EffectBuilder(Assets& assets) noexcept;
        ~EffectBuilder() = default;

        EffectBuilder& setBurstCount(std::unique_ptr<Distribution<uint32_t>> burst_count);
        EffectBuilder& setMaterial(std::shared_ptr<ms::Material> material);
        EffectBuilder& setMesh(std::shared_ptr<AbstractMesh> mesh);
        EffectBuilder& setDuration(std::chrono::duration<float> duration);
        EffectBuilder& setLocalPosition(const glm::vec3& local_position);
        EffectBuilder& setLocalRotation(const glm::quat& local_rotation);
        EffectBuilder& setSpawnMode(EmitterSpawn::Mode mode);
        EffectBuilder& setLocalSpace(bool _local_space);
        EffectBuilder& setEmitterType(AbstractEmitter::Type type);
        EffectBuilder& setMaxCount(uint64_t max_count);
        EffectBuilder& setSpawnRate(float spawn_rate);
        EffectBuilder& setLoops(int loops);

        EffectBuilder& create(std::string name);

        template<typename Emitter>
        EffectBuilder& createEmitter(const std::string& name);

        EffectBuilder& addInitialLocation(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addInitialRotation(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addInitialVelocity(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addInitialColor(std::unique_ptr<Distribution<glm::vec4>> distribution);
        EffectBuilder& addInitialSize(std::unique_ptr<Distribution<float>> distribution);
        EffectBuilder& addInitialSize(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addInitialAcceleration(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addInitialMeshLocation(std::shared_ptr<AbstractMesh> mesh, const glm::vec3& scale, const glm::vec3& rotation);
        EffectBuilder& addInitialMeshLocation(std::shared_ptr<AbstractModel> mesh, const glm::vec3& scale, const glm::vec3& rotation);
        EffectBuilder& addInitialMeshLocation(std::shared_ptr<AbstractMesh> mesh);
        EffectBuilder& addInitialMeshLocation(std::shared_ptr<AbstractModel> mesh);
        EffectBuilder& addMeshLocationAttachment(std::shared_ptr<AbstractMesh> mesh);
        EffectBuilder& addMeshLocationAttachment(std::shared_ptr<AbstractModel> mesh);
        EffectBuilder& addSubUV(const glm::vec2& size, float fps, const glm::vec2& frame_count);
        EffectBuilder& addVelocityByLife(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addColorByLife(std::unique_ptr<Distribution<glm::vec4>> distribution);
        EffectBuilder& addRotationRate(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addSizeByLife(std::unique_ptr<Distribution<float>> distribution);
        EffectBuilder& addSizeByLife(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addLifetime(std::unique_ptr<Distribution<float>> distribution);
        EffectBuilder& addTime();
        EffectBuilder& addBeamInitialTarget(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addBeamInitialDisplacement(std::unique_ptr<Distribution<float>> distribution);
        EffectBuilder& addBeamInitialOffset(std::unique_ptr<Distribution<float>> distribution);
        EffectBuilder& addBeamInitialRebuild(std::unique_ptr<Distribution<float>> distribution);

        EffectBuilder& addCustomMaterial(std::unique_ptr<Distribution<float>> p1,
                                         std::unique_ptr<Distribution<float>> p2,
                                         std::unique_ptr<Distribution<float>> p3,
                                         std::unique_ptr<Distribution<float>> p4);

        EffectBuilder& addCustomMaterialByLife(std::unique_ptr<Distribution<float>> p1,
                                               std::unique_ptr<Distribution<float>> p2,
                                               std::unique_ptr<Distribution<float>> p3,
                                               std::unique_ptr<Distribution<float>> p4);

        std::shared_ptr<EffectInstance> build();
    };
}
