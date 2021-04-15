#include <limitless/particle_system/effect_builder.hpp>

#include <limitless/material_system/material_builder.hpp>
#include <limitless/particle_system/effect_compiler.hpp>
#include <limitless/assets.hpp>

using namespace LimitlessEngine;

EffectBuilder::EffectBuilder(Context& _context, Assets& _assets) noexcept
    : context{_context}
    , assets {_assets} {
}

EffectBuilder& EffectBuilder::setEmitterType(EmitterType type) {
    effect->emitters.at(last_emitter)->type = type;
    return *this;
}

EffectBuilder& EffectBuilder::setLocalPosition(const glm::vec3& local_position) {
    effect->emitters.at(last_emitter)->local_position = local_position;
    return *this;
}

EffectBuilder& EffectBuilder::setLocalRotation(const glm::quat& local_rotation) {
    effect->emitters.at(last_emitter)->local_rotation = local_rotation;
    return *this;
}

EffectBuilder& EffectBuilder::setLocalSpace(bool local_space) {
    effect->emitters.at(last_emitter)->local_space = local_space;
    return *this;
}

EffectBuilder& EffectBuilder::setSpawnMode(EmitterSpawn::Mode mode) {
    effect->emitters.at(last_emitter)->spawn.mode = mode;
    return *this;
}

EffectBuilder& EffectBuilder::setMaxCount(uint64_t max_count) {
    effect->emitters.at(last_emitter)->spawn.max_count = max_count;
    return *this;
}

EffectBuilder& EffectBuilder::setSpawnRate(float spawn_rate) {
    effect->emitters.at(last_emitter)->spawn.spawn_rate = spawn_rate;
    return *this;
}

EffectBuilder& EffectBuilder::setBurstCount(std::unique_ptr<Distribution<uint32_t>> burst_count) {
    effect->emitters.at(last_emitter)->spawn.burst_count = std::move(burst_count);
    return *this;
}

EffectBuilder& EffectBuilder::setLoops(int loops) {
    effect->emitters.at(last_emitter)->spawn.loops = loops;
    return *this;
}

EffectBuilder& EffectBuilder::setDuration(std::chrono::duration<float> duration) {
    effect->emitters.at(last_emitter)->duration = duration;
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

EffectBuilder& EffectBuilder::addInitialLocation(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::InitialLocation] = std::make_unique<InitialLocation>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialRotation(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::InitialRotation] = std::make_unique<InitialRotation>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialVelocity(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::InitialVelocity] = std::make_unique<InitialVelocity>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialColor(std::unique_ptr<Distribution<glm::vec4>> distribution) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::InitialColor] = std::make_unique<InitialColor>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialSize(std::unique_ptr<Distribution<float>> distribution) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::InitialSize] = std::make_unique<InitialSize>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialAcceleration(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::InitialAcceleration] = std::make_unique<InitialAcceleration>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addMeshLocation(std::shared_ptr<AbstractMesh> mesh) {
    if (!mesh) {
        throw std::runtime_error{"Empty mesh cannot be set"};
    }

    effect->emitters.at(last_emitter)->modules[EmitterModuleType::MeshLocation] = std::make_unique<MeshLocation>(std::move(mesh));
    return *this;
}

EffectBuilder& EffectBuilder::addSubUV(const glm::vec2& size, float fps, const glm::vec2& frame_count) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::SubUV] = std::make_unique<SubUV>(size, fps, frame_count);
    return *this;
}

EffectBuilder& EffectBuilder::addVelocityByLife(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::VelocityByLife] = std::make_unique<VelocityByLife>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addColorByLife(std::unique_ptr<Distribution<glm::vec4>> distribution) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::ColorByLife] = std::make_unique<ColorByLife>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addRotationRate(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::RotationRate] = std::make_unique<RotationRate>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addSizeByLife(std::unique_ptr<Distribution<float>> distribution, float factor) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::SizeByLife] = std::make_unique<SizeByLife>(std::move(distribution), factor);
    return *this;
}

EffectBuilder& EffectBuilder::addCustomMaterial(std::unique_ptr<Distribution<float>> p1,
                                                std::unique_ptr<Distribution<float>> p2,
                                                std::unique_ptr<Distribution<float>> p3,
                                                std::unique_ptr<Distribution<float>> p4) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::CustomMaterial] = std::make_unique<CustomMaterialModule>(std::move(p1),
                                                                                                                           std::move(p2),
                                                                                                                           std::move(p3),
                                                                                                                           std::move(p4));
    return *this;
}

EffectBuilder& EffectBuilder::addCustomMaterialByLife(std::unique_ptr<Distribution<float>> p1,
                                                      std::unique_ptr<Distribution<float>> p2,
                                                      std::unique_ptr<Distribution<float>> p3,
                                                      std::unique_ptr<Distribution<float>> p4) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::CustomMaterialByLife] = std::make_unique<CustomMaterialModuleByLife>(std::move(p1),
                                                                                                                                       std::move(p2),
                                                                                                                                       std::move(p3),
                                                                                                                                       std::move(p4));
    return *this;
}

EffectBuilder& EffectBuilder::addLifetime(std::unique_ptr<Distribution<float>> distribution) {
    effect->emitters.at(last_emitter)->modules[EmitterModuleType::Lifetime] = std::make_unique<Lifetime>(std::move(distribution));
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
                MaterialBuilder::initializeMaterialBuffer(mesh_emitter.getMaterial(), assets.shaders.get(material_shaders.at(0), mesh_emitter.getEmitterType()));
                break;
            }
            case EmitterType::Sprite: {
                auto& sprite_emitter = effect->get<SpriteEmitter>(name);
                MaterialBuilder::initializeMaterialBuffer(sprite_emitter.getMaterial(), assets.shaders.get(material_shaders.at(0), sprite_emitter.getEmitterType()));
                break;
            }
        }
    }

    assets.effects.add(effect_name, effect);

    return effect;
}

EffectBuilder& EffectBuilder::setSpawn(EmitterSpawn&& spawn) {
    effect->emitters.at(last_emitter)->spawn = std::move(spawn);
    return *this;
}

EffectBuilder& EffectBuilder::create(std::string name) {
    effect = std::shared_ptr<EffectInstance>(new EffectInstance());
    effect->name = name;
    effect_name = std::move(name);
    return *this;
}

template<typename Emitter>
EffectBuilder& EffectBuilder::createEmitter(const std::string& name) {
    last_emitter = name;
    effect->emitters[name] = std::unique_ptr<Emitter>(new Emitter());
    return *this;
}


EffectBuilder& EffectBuilder::setModules(decltype(Emitter::modules)&& modules) {
    effect->emitters.at(last_emitter)->modules = std::move(modules);
    return *this;
}

namespace LimitlessEngine {
    template EffectBuilder& EffectBuilder::createEmitter<SpriteEmitter>(const std::string& name);
    template EffectBuilder& EffectBuilder::createEmitter<MeshEmitter>(const std::string& name);
}
