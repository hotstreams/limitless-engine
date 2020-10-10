#include <effect_renderer.hpp>

#include <effect_instance.hpp>
#include <emitter_renderer.hpp>
#include <material.hpp>
#include <shader_types.hpp>

using namespace GraphicsEngine;

void EffectRenderer::visitEmitters(const std::vector<AbstractInstance*>& instances, EmitterVisiter& emitter_visiter) noexcept {
    auto effect_instance_visiter = [] (const EffectInstance& effect_instance, EmitterVisiter& visiter) {
        for (const auto& [name, emitter] : effect_instance) {
            emitter->accept(visiter);
        }
    };

    // iterates through effect instance attachments
    for (const auto* instance : instances)
        for (const auto& effect_attachment : *instance)
            effect_instance_visiter(*effect_attachment.instance, emitter_visiter);

    // iterates through effect instances itself
    for (const auto* instance : instances) {
        if (instance->getShaderType() == ModelShader::Effect) {
            effect_instance_visiter(static_cast<const EffectInstance&>(*instance), emitter_visiter);
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
    context.enable(GL_VERTEX_PROGRAM_POINT_SIZE);
}
