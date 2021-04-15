#pragma once

#include <limitless/instances/effect_instance.hpp>
#include <limitless/particle_system/mesh_emitter.hpp>
#include <limitless/shader_types.hpp>

namespace LimitlessEngine {
    class Material;
    class Context;

    class EffectBuilder {
    private:
        std::shared_ptr<EffectInstance> effect;
        std::string effect_name;
        std::string last_emitter;

        Context& context;
        Assets& assets;

        EffectBuilder& setModules(decltype(Emitter::modules)&& modules);
        friend class EmitterSerializer;
    public:
        EffectBuilder(Context& context, Assets& assets) noexcept;
        ~EffectBuilder() = default;

        EffectBuilder& setBurstCount(std::unique_ptr<Distribution<uint32_t>> burst_count);
        EffectBuilder& setMaterial(std::shared_ptr<Material> material);
        EffectBuilder& setMesh(std::shared_ptr<AbstractMesh> mesh);
        EffectBuilder& setDuration(std::chrono::duration<float> duration);
        EffectBuilder& setLocalPosition(const glm::vec3& local_position);
        EffectBuilder& setLocalRotation(const glm::quat& local_rotation);
        EffectBuilder& setSpawnMode(EmitterSpawn::Mode mode);
        EffectBuilder& setLocalSpace(bool _local_space);
        EffectBuilder& setEmitterType(EmitterType type);
        EffectBuilder& setMaxCount(uint64_t max_count);
        EffectBuilder& setSpawnRate(float spawn_rate);
        EffectBuilder& setSpawn(EmitterSpawn&& spawn);
        EffectBuilder& setLoops(int loops);

        EffectBuilder& create(std::string name);

        template<typename Emitter>
        EffectBuilder& createEmitter(const std::string& name);

        EffectBuilder& addInitialLocation(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addInitialRotation(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addInitialVelocity(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addInitialColor(std::unique_ptr<Distribution<glm::vec4>> distribution);
        EffectBuilder& addInitialSize(std::unique_ptr<Distribution<float>> distribution);
        EffectBuilder& addInitialAcceleration(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addMeshLocation(std::shared_ptr<AbstractMesh> mesh);
        EffectBuilder& addSubUV(const glm::vec2& size, float fps, const glm::vec2& frame_count);
        EffectBuilder& addVelocityByLife(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addColorByLife(std::unique_ptr<Distribution<glm::vec4>> distribution);
        EffectBuilder& addRotationRate(std::unique_ptr<Distribution<glm::vec3>> distribution);
        EffectBuilder& addSizeByLife(std::unique_ptr<Distribution<float>> distribution, float factor);
        EffectBuilder& addLifetime(std::unique_ptr<Distribution<float>> distribution);

        EffectBuilder& addCustomMaterial(std::unique_ptr<Distribution<float>> p1,
                                         std::unique_ptr<Distribution<float>> p2,
                                         std::unique_ptr<Distribution<float>> p3,
                                         std::unique_ptr<Distribution<float>> p4);

        EffectBuilder& addCustomMaterialByLife(std::unique_ptr<Distribution<float>> p1,
                                               std::unique_ptr<Distribution<float>> p2,
                                               std::unique_ptr<Distribution<float>> p3,
                                               std::unique_ptr<Distribution<float>> p4);

        std::shared_ptr<EffectInstance> build(const MaterialShaders& material_shaders = {MaterialShader::Forward, MaterialShader::DirectionalShadow});
    };
}
