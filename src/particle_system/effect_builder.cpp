#include <particle_system/effect_builder.hpp>

#include <effect_instance.hpp>
#include <particle_system/mesh_emitter.hpp>
#include <particle_system/effect_compiler.hpp>
#include <assets.hpp>

using namespace GraphicsEngine;

EffectBuilder& EffectBuilder::setEmitterType(EmitterType type) noexcept {
    effect->emitters[last_emitter]->type = type;
    return *this;
}

EffectBuilder& EffectBuilder::setLocalPosition(const glm::vec3& local_position) noexcept {
    effect->emitters[last_emitter]->local_position = local_position;
    return *this;
}

EffectBuilder& EffectBuilder::setLocalRotation(const glm::vec3& local_rotation) noexcept {
    effect->emitters[last_emitter]->local_rotation = local_rotation;
    return *this;
}

EffectBuilder& EffectBuilder::setLocalSpace(bool local_space) noexcept {
    effect->emitters[last_emitter]->local_space = local_space;
    return *this;
}

EffectBuilder& EffectBuilder::setSpawnMode(EmitterSpawn::Mode mode) noexcept {
    effect->emitters[last_emitter]->spawn.mode = mode;
    return *this;
}

EffectBuilder& EffectBuilder::setMaxCount(uint64_t max_count) noexcept {
    effect->emitters[last_emitter]->spawn.max_count = max_count;
    return *this;
}

EffectBuilder& EffectBuilder::setSpawnRate(float spawn_rate) noexcept {
    effect->emitters[last_emitter]->spawn.spawn_rate = spawn_rate;
    return *this;
}

EffectBuilder& EffectBuilder::setBurstCount(std::unique_ptr<Distribution<uint32_t>> burst_count) noexcept {
    effect->emitters[last_emitter]->spawn.burst_count = std::move(burst_count);
    return *this;
}

EffectBuilder& EffectBuilder::setLoops(int loops) noexcept {
    effect->emitters[last_emitter]->spawn.loops = loops;
    return *this;
}

EffectBuilder& EffectBuilder::setDuration(std::chrono::duration<float> duration) noexcept {
    effect->emitters[last_emitter]->duration = duration;
    return *this;
}

EffectBuilder& EffectBuilder::setMaterial(const std::shared_ptr<Material>& material) noexcept {
    effect->get<SpriteEmitter>(last_emitter).material = material;
    return *this;
}

EffectBuilder& EffectBuilder::setMesh(const std::shared_ptr<AbstractMesh>& mesh) noexcept {
    effect->get<MeshEmitter>(last_emitter).mesh = mesh;
    return *this;
}

std::shared_ptr<EffectInstance> EffectBuilder::build() {
    EffectCompiler compiler;
    compiler.compile(*effect);
    auto compiled = std::shared_ptr<EffectInstance>(std::move(effect));
    assets.effects.add(effect_name, compiled);
    return compiled;
}

EffectBuilder& EffectBuilder::create(std::string name) {
    effect = std::unique_ptr<EffectInstance>(new EffectInstance());
    effect_name = std::move(name);
    return *this;
}
