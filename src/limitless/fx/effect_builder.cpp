#include <limitless/fx/effect_builder.hpp>

#include <limitless/instances/effect_instance.hpp>

#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>

#include <limitless/ms/material_builder.hpp>
#include <limitless/fx/effect_compiler.hpp>
#include <limitless/assets.hpp>

#include <limitless/fx/modules/modules.hpp>

using namespace Limitless::fx;

EffectBuilder::EffectBuilder(Assets& _assets) noexcept
    : assets {_assets} {
}

EffectBuilder& EffectBuilder::setEmitterType(AbstractEmitter::Type type) {
    effect->emitters.at(last_emitter)->type = type;
    return *this;
}

EffectBuilder& EffectBuilder::setLocalPosition(const glm::vec3& local_position) {
    effect->emitters.at(last_emitter)->getLocalPosition() = local_position;
    return *this;
}

EffectBuilder& EffectBuilder::setLocalRotation(const glm::quat& local_rotation) {
    effect->emitters.at(last_emitter)->getLocalRotation() = local_rotation;
    return *this;
}

EffectBuilder& EffectBuilder::setLocalSpace(bool local_space) {
    effect->emitters.at(last_emitter)->getLocalSpace() = local_space;
    return *this;
}

EffectBuilder& EffectBuilder::setSpawnMode(EmitterSpawn::Mode mode) {
    effect->emitters.at(last_emitter)->getSpawn().mode = mode;
    if (mode == EmitterSpawn::Mode::Burst) {
        effect->emitters.at(last_emitter)->getSpawn().burst = EmitterSpawn::Burst {};
    } else {
        effect->emitters.at(last_emitter)->getSpawn().burst = std::nullopt;
    }
    return *this;
}

EffectBuilder& EffectBuilder::setMaxCount(uint64_t max_count) {
    effect->emitters.at(last_emitter)->getSpawn().max_count = max_count;
    return *this;
}

EffectBuilder& EffectBuilder::setSpawnRate(float spawn_rate) {
    effect->emitters.at(last_emitter)->getSpawn().spawn_rate = spawn_rate;
    return *this;
}

EffectBuilder& EffectBuilder::setBurstCount(std::unique_ptr<Distribution<uint32_t>> burst_count) {
    effect->emitters.at(last_emitter)->getSpawn().burst->burst_count = std::move(burst_count);
    return *this;
}

EffectBuilder& EffectBuilder::setLoops(int loops) {
    effect->emitters.at(last_emitter)->getSpawn().burst->loops = loops;
    return *this;
}

EffectBuilder& EffectBuilder::setDuration(std::chrono::duration<float> duration) {
    effect->emitters.at(last_emitter)->getDuration() = duration;
    return *this;
}

EffectBuilder& EffectBuilder::setMaterial(std::shared_ptr<ms::Material> material) {
    if (!material) {
        throw std::runtime_error{"Cannot set empty material for emitter!"};
    }

//    if (material->contains("time")) {
//        throw std::runtime_error{"Uniform Time cannot be used in emitter material. Use TimeModule instead!"};
//    }

    switch (effect->emitters.at(last_emitter)->getType()) {
        case AbstractEmitter::Type::Sprite:
            effect->get<SpriteEmitter>(last_emitter).material = std::move(material);
            break;
        case AbstractEmitter::Type::Mesh:
            effect->get<MeshEmitter>(last_emitter).material = std::move(material);
            break;
        case AbstractEmitter::Type::Beam:
            effect->get<BeamEmitter>(last_emitter).material = std::move(material);
            break;
    }

    return *this;
}

EffectBuilder& EffectBuilder::setMesh(std::shared_ptr<AbstractMesh> mesh) {
    if (!mesh) {
        throw std::runtime_error{"Cannot set empty mesh for emitter!"};
    }

    if (effect->emitters[last_emitter]->getType() != AbstractEmitter::Type::Mesh) {
        throw std::runtime_error{"Cannot set mesh for not MeshEmitter!"};
    }

    effect->get<MeshEmitter>(last_emitter).mesh = std::move(mesh);
    return *this;
}

template<template<typename P> class M, typename... Args>
void EffectBuilder::addModule(Args&&... args) {
    switch (effect->emitters.at(last_emitter)->getType()) {
        case AbstractEmitter::Type::Sprite:
            effect->get<SpriteEmitter>(last_emitter).modules.emplace(new M<SpriteParticle>(std::forward<Args>(args)...));
            break;
        case AbstractEmitter::Type::Mesh:
            effect->get<MeshEmitter>(last_emitter).modules.emplace(new M<MeshParticle>(std::forward<Args>(args)...));
            break;
        case AbstractEmitter::Type::Beam:
            effect->get<BeamEmitter>(last_emitter).modules.emplace(new M<BeamParticle>(std::forward<Args>(args)...));
            break;
    }
}

EffectBuilder& EffectBuilder::addInitialLocation(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    addModule<InitialLocation>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialRotation(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    addModule<InitialRotation>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialVelocity(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    addModule<InitialVelocity>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialColor(std::unique_ptr<Distribution<glm::vec4>> distribution) {
    addModule<InitialColor>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialSize(std::unique_ptr<Distribution<float>> distribution) {
    switch (effect->emitters.at(last_emitter)->getType()) {
        case AbstractEmitter::Type::Sprite:
            effect->get<SpriteEmitter>(last_emitter).modules.emplace(new InitialSize<SpriteParticle>(std::move(distribution)));
            break;
        case AbstractEmitter::Type::Beam:
            effect->get<BeamEmitter>(last_emitter).modules.emplace(new InitialSize<BeamParticle>(std::move(distribution)));
            break;
        case AbstractEmitter::Type::Mesh:
            throw std::runtime_error("Bad addInitialSize argument specified!");
    }
    return *this;
}

EffectBuilder& EffectBuilder::addInitialSize(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    switch (effect->emitters.at(last_emitter)->getType()) {
        case AbstractEmitter::Type::Sprite:
        case AbstractEmitter::Type::Beam:
            throw std::runtime_error("Bad addInitialSize argument specified!");
        case AbstractEmitter::Type::Mesh:
            effect->get<MeshEmitter>(last_emitter).modules.emplace(new InitialSize<MeshParticle>(std::move(distribution)));
            break;
    }
    return *this;
}

EffectBuilder& EffectBuilder::addInitialAcceleration(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    addModule<InitialAcceleration>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialMeshLocation(std::shared_ptr<AbstractMesh> mesh, const glm::vec3& scale, const glm::vec3& rotation) {
    if (!mesh) {
        throw std::runtime_error{"Empty mesh cannot be set"};
    }

    addModule<InitialMeshLocation>(std::move(mesh), scale, rotation);
    return *this;
}

EffectBuilder& EffectBuilder::addInitialMeshLocation(std::shared_ptr<AbstractModel> mesh, const glm::vec3& scale, const glm::vec3& rotation) {
    if (!mesh) {
        throw std::runtime_error{"Empty mesh cannot be set"};
    }

    addModule<InitialMeshLocation>(std::move(mesh), scale, rotation);
    return *this;
}

EffectBuilder& EffectBuilder::addInitialMeshLocation(std::shared_ptr<AbstractMesh> mesh) {
    if (!mesh) {
        throw std::runtime_error{"Empty mesh cannot be set"};
    }

    addModule<InitialMeshLocation>(std::move(mesh));
    return *this;
}

EffectBuilder& EffectBuilder::addInitialMeshLocation(std::shared_ptr<AbstractModel> mesh) {
    if (!mesh) {
        throw std::runtime_error{"Empty mesh cannot be set"};
    }

    addModule<InitialMeshLocation>(std::move(mesh));
    return *this;
}

EffectBuilder& EffectBuilder::addMeshLocationAttachment(std::shared_ptr<AbstractMesh> mesh) {
    if (!mesh) {
        throw std::runtime_error{"Empty mesh cannot be set"};
    }

    addModule<MeshLocationAttachment>(std::move(mesh));
    return *this;
}

EffectBuilder& EffectBuilder::addMeshLocationAttachment(std::shared_ptr<AbstractModel> mesh) {
    if (!mesh) {
        throw std::runtime_error{"Empty mesh cannot be set"};
    }

    addModule<MeshLocationAttachment>(std::move(mesh));
    return *this;
}

EffectBuilder& EffectBuilder::addSubUV(const glm::vec2& size, float fps, const glm::vec2& frame_count) {
    addModule<SubUV>(size, fps, frame_count);
    return *this;
}

EffectBuilder& EffectBuilder::addVelocityByLife(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    addModule<VelocityByLife>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addColorByLife(std::unique_ptr<Distribution<glm::vec4>> distribution) {
    addModule<ColorByLife>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addRotationRate(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    addModule<RotationRate>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addTime() {
    addModule<Time>();
    return *this;
}

EffectBuilder& EffectBuilder::addBeamSpeed(std::unique_ptr<Distribution<float>> distribution) {
    if (effect->emitters.at(last_emitter)->getType() != AbstractEmitter::Type::Beam) {
        throw std::runtime_error{"Cannot set to not BeamEmitter"};
    }

    effect->get<BeamEmitter>(last_emitter).modules.emplace(new BeamSpeed<BeamParticle>(std::move(distribution)));
    return *this;
}

EffectBuilder& EffectBuilder::addSizeByLife(std::unique_ptr<Distribution<float>> distribution) {
    switch (effect->emitters.at(last_emitter)->getType()) {
        case AbstractEmitter::Type::Sprite:
            effect->get<SpriteEmitter>(last_emitter).modules.emplace(new SizeByLife<SpriteParticle>(std::move(distribution)));
            break;
        case AbstractEmitter::Type::Beam:
            effect->get<BeamEmitter>(last_emitter).modules.emplace(new SizeByLife<BeamParticle>(std::move(distribution)));
            break;
        case AbstractEmitter::Type::Mesh:
            throw std::runtime_error("Bad addSizeByLife argument specified!");
    }
    return *this;
}

EffectBuilder& EffectBuilder::addSizeByLife(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    switch (effect->emitters.at(last_emitter)->getType()) {
        case AbstractEmitter::Type::Sprite:
        case AbstractEmitter::Type::Beam:
            throw std::runtime_error("Bad addSizeByLife argument specified!");
        case AbstractEmitter::Type::Mesh:
            effect->get<MeshEmitter>(last_emitter).modules.emplace(new SizeByLife<MeshParticle>(std::move(distribution)));
            break;
    }
    return *this;
}

EffectBuilder& EffectBuilder::addCustomMaterial(std::unique_ptr<Distribution<float>> p1,
                                                std::unique_ptr<Distribution<float>> p2,
                                                std::unique_ptr<Distribution<float>> p3,
                                                std::unique_ptr<Distribution<float>> p4) {
    addModule<CustomMaterial>(std::move(p1), std::move(p2), std::move(p3), std::move(p4));
    return *this;
}

EffectBuilder& EffectBuilder::addCustomMaterialByLife(std::unique_ptr<Distribution<float>> p1,
                                                      std::unique_ptr<Distribution<float>> p2,
                                                      std::unique_ptr<Distribution<float>> p3,
                                                      std::unique_ptr<Distribution<float>> p4) {
    addModule<CustomMaterialByLife>(std::move(p1), std::move(p2), std::move(p3), std::move(p4));
    return *this;
}

EffectBuilder& EffectBuilder::addLifetime(std::unique_ptr<Distribution<float>> distribution) {
    addModule<Lifetime>(std::move(distribution));
    return *this;
}

EffectBuilder& EffectBuilder::addBeamInitialTarget(std::unique_ptr<Distribution<glm::vec3>> distribution) {
    if (effect->emitters.at(last_emitter)->getType() != AbstractEmitter::Type::Beam) {
        throw std::runtime_error{"Cannot set to not BeamEmitter"};
    }

    effect->get<BeamEmitter>(last_emitter).modules.emplace(new BeamTarget<BeamParticle>(std::move(distribution)));

    return *this;
}

EffectBuilder& EffectBuilder::addBeamInitialDisplacement(std::unique_ptr<Distribution<float>> distribution) {
    if (effect->emitters.at(last_emitter)->getType() != AbstractEmitter::Type::Beam) {
        throw std::runtime_error{"Cannot set to not BeamEmitter"};
    }

    effect->get<BeamEmitter>(last_emitter).modules.emplace(new BeamDisplacement<BeamParticle>(std::move(distribution)));

    return *this;
}

EffectBuilder& EffectBuilder::addBeamInitialOffset(std::unique_ptr<Distribution<float>> distribution) {
    if (effect->emitters.at(last_emitter)->getType() != AbstractEmitter::Type::Beam) {
        throw std::runtime_error{"Cannot set to not BeamEmitter"};
    }

    effect->get<BeamEmitter>(last_emitter).modules.emplace(new BeamOffset<BeamParticle>(std::move(distribution)));

    return *this;
}

EffectBuilder& EffectBuilder::addBeamInitialRebuild(std::unique_ptr<Distribution<float>> distribution) {
    if (effect->emitters.at(last_emitter)->getType() != AbstractEmitter::Type::Beam) {
        throw std::runtime_error{"Cannot set to not BeamEmitter"};
    }

    effect->get<BeamEmitter>(last_emitter).modules.emplace(new BeamRebuild<BeamParticle>(std::move(distribution)));

    return *this;
}

std::shared_ptr<Limitless::EffectInstance> EffectBuilder::build() {
    for (const auto& [name, emitter] : effect->getEmitters()) {
        switch (emitter->getType()) {
            case AbstractEmitter::Type::Sprite: {
                auto& sprite_emitter = effect->get<SpriteEmitter>(name);
                sprite_emitter.unique_shader = getUniqueEmitterShader(sprite_emitter);
                break;
            }
            case AbstractEmitter::Type::Mesh: {
                auto& mesh_emitter = effect->get<MeshEmitter>(name);
                mesh_emitter.unique_shader = getUniqueEmitterShader(mesh_emitter);
                break;
            }
            case AbstractEmitter::Type::Beam: {
                auto& beam_emitter = effect->get<BeamEmitter>(name);
                beam_emitter.unique_shader = getUniqueEmitterShader(beam_emitter);
                break;
            }
        }
    }

    assets.effects.add(effect_name, effect);

    return effect;
}

EffectBuilder& EffectBuilder::setSpawn(EmitterSpawn&& spawn) {
    switch (effect->emitters.at(last_emitter)->getType()) {
        case AbstractEmitter::Type::Sprite:
            effect->get<SpriteEmitter>(last_emitter).spawn = std::move(spawn);
            break;
        case AbstractEmitter::Type::Mesh:
            effect->get<MeshEmitter>(last_emitter).spawn = std::move(spawn);
            break;
        case AbstractEmitter::Type::Beam:
            effect->get<BeamEmitter>(last_emitter).spawn = std::move(spawn);
            break;
    }
    return *this;
}

EffectBuilder& EffectBuilder::create(std::string name) {
    effect = std::shared_ptr<EffectInstance>(new EffectInstance());
    effect->name = name;
    effect_name = std::move(name);
    return *this;
}

template<typename Emitter>
EffectBuilder& EffectBuilder::setModules(decltype(Emitter::modules)&& modules) {
    if constexpr (std::is_same_v<Emitter, SpriteEmitter>) {
        effect->get<SpriteEmitter>(last_emitter).modules = std::move(modules);
    }

    if constexpr (std::is_same_v<Emitter, MeshEmitter>) {
        effect->get<MeshEmitter>(last_emitter).modules = std::move(modules);
    }

    if constexpr (std::is_same_v<Emitter, BeamEmitter>) {
        effect->get<BeamEmitter>(last_emitter).modules = std::move(modules);
    }
    return *this;
}

template<typename Emitter>
EffectBuilder& EffectBuilder::createEmitter(const std::string& name) {
    last_emitter = name;
    effect->emitters[name] = std::unique_ptr<Emitter>(new Emitter());

    if constexpr (std::is_same_v<Emitter, BeamEmitter>) {
        effect->get<BeamEmitter>(last_emitter).modules.emplace(new BeamBuilder<BeamParticle>());
    }

    return *this;
}

template<typename Emitter>
UniqueEmitterShader EffectBuilder::getUniqueEmitterShader(const Emitter& emitter) const noexcept {
    std::set<ModuleType> module_type;
    for (const auto& module : emitter.modules) {
        module_type.emplace(module->getType());
    }
    return { emitter.getType(), module_type, emitter.getMaterial().getShaderIndex() };
}

namespace Limitless::fx {
    template EffectBuilder& EffectBuilder::createEmitter<SpriteEmitter>(const std::string& name);
    template EffectBuilder& EffectBuilder::createEmitter<MeshEmitter>(const std::string& name);
    template EffectBuilder& EffectBuilder::createEmitter<BeamEmitter>(const std::string& name);

    template EffectBuilder& EffectBuilder::setModules<SpriteEmitter>(decltype(SpriteEmitter::modules)&& modules);
    template EffectBuilder& EffectBuilder::setModules<MeshEmitter>(decltype(MeshEmitter::modules)&& modules);
    template EffectBuilder& EffectBuilder::setModules<BeamEmitter>(decltype(BeamEmitter::modules)&& modules);
}
