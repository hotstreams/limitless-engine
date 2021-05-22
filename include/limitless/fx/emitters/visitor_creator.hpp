#pragma once

#include <limitless/fx/emitters/emitter_visitor.hpp>
#include <limitless/fx/renderers/sprite_emitter_renderer.hpp>
#include <limitless/fx/renderers/mesh_emitter_renderer.hpp>
#include <limitless/fx/renderers/beam_emitter_renderer.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>

namespace Limitless::fx {
    class EmitterRendererCreator : public EmitterVisitor {
    private:
        std::map<UniqueEmitterRenderer, std::unique_ptr<AbstractEmitterRenderer>>& renderers;
    public:
        explicit EmitterRendererCreator(decltype(renderers) renderers) noexcept
            : renderers {renderers} {}
        ~EmitterRendererCreator() override = default;

        void visit(const SpriteEmitter& emitter) noexcept override {
            if (!renderers.count(emitter.getUniqueRendererType())) {
                auto type = emitter.getUniqueRendererType();
                type.material = std::make_shared<ms::Material>(*type.material);
                renderers.emplace(type, new EmitterRenderer<SpriteParticle>(emitter));
            }
        }

        void visit(const MeshEmitter& emitter) noexcept override {
            if (!renderers.count(emitter.getUniqueRendererType())) {
                auto type = emitter.getUniqueRendererType();
                type.material = std::make_shared<ms::Material>(*type.material);
                renderers.emplace(type, new EmitterRenderer<MeshParticle>(emitter));
            }
        }

        void visit(const BeamEmitter& emitter) noexcept override {
            if (!renderers.count(emitter.getUniqueRendererType())) {
                auto type = emitter.getUniqueRendererType();
                type.material = std::make_shared<ms::Material>(*type.material);
                renderers.emplace(type, new EmitterRenderer<BeamParticle>(emitter));
            }
        }
    };
}