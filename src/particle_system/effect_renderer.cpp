#include <particle_system/effect_renderer.hpp>

#include <effect_instance.hpp>
#include <particle_system/emitter_renderer.hpp>
#include <material_system/material.hpp>
#include <shader_types.hpp>

using namespace LimitlessEngine;

void EffectRenderer::visitEmitters(const std::vector<AbstractInstance*>& instances, EmitterVisitor& emitter_visitor) noexcept {
    auto effect_instance_visitor = [] (const EffectInstance& effect_instance, EmitterVisitor& visitor) {
        for (const auto& [name, emitter] : effect_instance) {
            emitter->accept(visitor);
        }
    };

    // iterates through effect instance attachments
    for (const auto* instance : instances)
        for (const auto& effect_attachment : *instance)
            effect_instance_visitor(*effect_attachment.instance, emitter_visitor);

    // iterates through effect instances itself
    for (const auto* instance : instances) {
        if (instance->getShaderType() == ModelShader::Effect) {
            effect_instance_visitor(static_cast<const EffectInstance&>(*instance), emitter_visitor);
        }
    }
}

void EffectRenderer::addStorage(const std::vector<AbstractInstance*>& instances) noexcept {
    EmitterStorageFiller filler{sprite_renderer, mesh_renderer};
    visitEmitters(instances, filler);
}

void EffectRenderer::update(const std::vector<AbstractInstance*>& instances) {
    addStorage(instances);

    for (auto& [emitter, storage] : sprite_renderer) {
        SpriteParticleCollector collector{emitter};
        visitEmitters(instances, collector);
        storage.update(collector);
    }

    for (auto& [emitter, storage] : mesh_renderer) {
        MeshParticleCollector collector{emitter};
        visitEmitters(instances, collector);
        storage.update(collector);
    }
}

void EffectRenderer::draw(Blending blending) {
    for (auto& [emitter, storage] : sprite_renderer) {
        if (emitter.material->getBlending() == blending)
            storage.draw(emitter);
    }
    for (auto& [emitter, storage] : mesh_renderer) {
        if (emitter.material->getBlending() == blending)
            storage.draw(emitter);
    }
}

EffectRenderer::EffectRenderer(Context &context) noexcept {
    context.enable(Enable::ProgramPointSize);
}
