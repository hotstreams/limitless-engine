#include <limitless/fx/effect_renderer.hpp>

#include <limitless/instances/effect_instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/fx/emitters/emitter.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <limitless/fx/emitters/emitter_visitor.hpp>
#include <limitless/fx/emitters/visitor_creator.hpp>

using namespace Limitless::fx;

void EffectRenderer::visitEmitters(const Instances& instances, EmitterVisitor& emitter_visitor) noexcept {
    auto effect_instance_visitor = [] (const EffectInstance& effect_instance, EmitterVisitor& visitor) {
        for (const auto& [name, emitter] : effect_instance) {
            emitter->accept(visitor);
        }
    };

    // iterates through effect instance attachments
    for (const auto& instance : instances)
        for (const auto& effect_attachment : instance.get().getAttachedEffects())
            effect_instance_visitor(*effect_attachment.instance, emitter_visitor);

    // iterates through effect instances itself
    for (const auto& instance : instances) {
        if (instance.get().getShaderType() == ModelShader::Effect) {
            effect_instance_visitor(static_cast<const EffectInstance&>(instance.get()), emitter_visitor);
        }
    }
}

void EffectRenderer::updateRenderers(const Instances& instances) noexcept {
    EmitterRendererCreator creator {renderers};
    visitEmitters(instances, creator);
}

void EffectRenderer::update(const Instances& instances) {
    updateRenderers(instances);

    for (const auto& [type, renderer] : renderers) {
        switch (type.emitter_type) {
            case AbstractEmitter::Type::Sprite: {
                ParticleCollector<SpriteParticle> collector {type};
                visitEmitters(instances, collector);
                static_cast<EmitterRenderer<SpriteParticle>&>(*renderer).update(collector);
                break;
            }
            case AbstractEmitter::Type::Mesh: {
                ParticleCollector<MeshParticle> collector {type};
                visitEmitters(instances, collector);
                static_cast<EmitterRenderer<MeshParticle>&>(*renderer).update(collector);
                break;
            }
            case AbstractEmitter::Type::Beam: {
                ParticleCollector<BeamParticleMapping> collector {type};
                visitEmitters(instances, collector);
                static_cast<EmitterRenderer<BeamParticle>&>(*renderer).update(collector);
                break;
            }
        }

    }
}

void EffectRenderer::draw(Context& ctx, const Assets& assets, ShaderPass shader, ms::Blending blending, const UniformSetter& setter) {
    for (const auto& [type, renderer] : renderers) {
        switch (type.emitter_type) {
            case AbstractEmitter::Type::Sprite: {
                auto& sprite_renderer = static_cast<EmitterRenderer<SpriteParticle>&>(*renderer);
                sprite_renderer.draw(ctx, assets, shader, blending, setter);
                break;
            }
            case AbstractEmitter::Type::Mesh: {
                auto& mesh_renderer = static_cast<EmitterRenderer<MeshParticle>&>(*renderer);
                mesh_renderer.draw(ctx, assets, shader, blending, setter);
                break;
            }
            case AbstractEmitter::Type::Beam: {
                auto& beam_renderer = static_cast<EmitterRenderer<BeamParticle>&>(*renderer);
                beam_renderer.draw(ctx, assets, shader, blending, setter);
                break;
            }
        }
    }
}

EffectRenderer::EffectRenderer(Context& context) noexcept {
    context.enable(Capabilities::ProgramPointSize);
}
