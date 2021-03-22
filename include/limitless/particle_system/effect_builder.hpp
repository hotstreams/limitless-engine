#pragma once

#include <limitless/effect_instance.hpp>
#include <limitless/particle_system/mesh_emitter.hpp>

namespace LimitlessEngine {
    class Material;

    class EffectBuilder {
    private:
        std::unique_ptr<EffectInstance> effect;
        std::string effect_name;
        std::string last_emitter;

        EffectBuilder& setModules(decltype(Emitter::modules)&& modules) noexcept;
        friend class EmitterSerializer;
    public:
        EffectBuilder() = default;
        ~EffectBuilder() = default;

        EffectBuilder& setBurstCount(std::unique_ptr<Distribution<uint32_t>> burst_count) noexcept;
        EffectBuilder& setMaterial(const std::shared_ptr<Material>& material) noexcept;
        EffectBuilder& setMesh(const std::shared_ptr<AbstractMesh>& mesh) noexcept;
        EffectBuilder& setDuration(std::chrono::duration<float> duration) noexcept;
        EffectBuilder& setLocalPosition(const glm::vec3& local_position) noexcept;
        EffectBuilder& setLocalRotation(const glm::vec3& local_rotation) noexcept;
        EffectBuilder& setSpawnMode(EmitterSpawn::Mode mode) noexcept;
        EffectBuilder& setLocalSpace(bool _local_space) noexcept;
        EffectBuilder& setEmitterType(EmitterType type) noexcept;
        EffectBuilder& setMaxCount(uint64_t max_count) noexcept;
        EffectBuilder& setSpawnRate(float spawn_rate) noexcept;
        EffectBuilder& setSpawn(EmitterSpawn&& spawn) noexcept;
        EffectBuilder& setLoops(int loops) noexcept;

        EffectBuilder& create(std::string name);

        template<typename T>
        EffectBuilder& createEmitter(std::string name) {
            last_emitter = name;
            effect->emitters.emplace(std::move(name), new T());
            return *this;
        }

        template<typename T, typename... Args>
        EffectBuilder& addModule(EmitterModuleType module_type, Args&&... args) {
            effect->emitters[last_emitter]->modules.emplace(module_type, new T{std::forward<Args>(args)...});
            return *this;
        }

        std::shared_ptr<EffectInstance> build();
    };
}
