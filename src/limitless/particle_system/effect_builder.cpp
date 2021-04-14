#include <limitless/particle_system/effect_builder.hpp>

#include <limitless/material_system/material_builder.hpp>
#include <limitless/particle_system/effect_compiler.hpp>
#include <limitless/material_system/material.hpp>
#include <limitless/assets.hpp>

using namespace LimitlessEngine;

EffectBuilder::EffectBuilder(Context& _context, Assets& _assets) noexcept
    : context{_context}
    , assets {_assets} {
}

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

EffectBuilder& EffectBuilder::setMaterial(std::shared_ptr<Material> material) {
    if (!material) {
        throw std::runtime_error{"Cannot set empty material for emitter!"};
    }

    effect->get<SpriteEmitter>(last_emitter).material = std::move(material);
    return *this;
}

EffectBuilder& EffectBuilder::setMesh(std::shared_ptr<AbstractMesh> mesh) {
    if (!mesh) {
        throw std::runtime_error{"Cannot set empty mesh for emitter!"};
    }

    if (effect->emitters[last_emitter]->getType() != EmitterType::Mesh) {
        throw std::runtime_error{"Cannot set mesh for not MeshEmitter!"};
    }

    effect->get<MeshEmitter>(last_emitter).mesh =std::move(mesh);
    return *this;
}

std::shared_ptr<EffectInstance> EffectBuilder::build(const MaterialShaders& material_shaders) {
    EffectCompiler compiler {context, assets};
    for (const auto& mat_shader : material_shaders) {
        compiler.compile(*effect, mat_shader);
    }

    for (const auto& [name, emitter] : *effect) {
        switch (emitter->getType()) {
            case EmitterType::Mesh: {
                auto& mesh_emitter = effect->get<MeshEmitter>(name);
                if (!mesh_emitter.getMaterial().material_buffer) {
                    // initializes uniform material buffer using program shader introspection
                    MaterialBuilder::initializeMaterialBuffer(mesh_emitter.getMaterial(), assets.shaders.get(material_shaders.at(0), mesh_emitter.getEmitterType()));
                }
                break;
            }
            case EmitterType::Sprite: {
                auto& sprite_emitter = effect->get<SpriteEmitter>(name);
                if (!sprite_emitter.getMaterial().material_buffer) {
                    // initializes uniform material buffer using program shader introspection
                    MaterialBuilder::initializeMaterialBuffer(sprite_emitter.getMaterial(), assets.shaders.get(material_shaders.at(0), sprite_emitter.getEmitterType()));
                }
                break;
            }
        }
    }

    assets.effects.add(effect_name, effect);

    return effect;
}

EffectBuilder& EffectBuilder::create(std::string name) {
    effect = std::shared_ptr<EffectInstance>(new EffectInstance());
    effect->name = name;
    effect_name = std::move(name);
    return *this;
}

EffectBuilder& EffectBuilder::setSpawn(EmitterSpawn&& spawn) noexcept {
    effect->emitters[last_emitter]->spawn = std::move(spawn);
    return *this;
}

EffectBuilder& EffectBuilder::setModules(decltype(Emitter::modules)&& modules) noexcept {
    effect->emitters[last_emitter]->modules = std::move(modules);
    return *this;
}
