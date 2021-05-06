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
        std::map<UniqueEmitter, std::unique_ptr<AbstractEmitterRenderer>>& renderers;
    public:
        explicit EmitterRendererCreator(decltype(renderers) renderers) noexcept
            : renderers {renderers} {}
        ~EmitterRendererCreator() override = default;

        void visit(const SpriteEmitter& emitter) noexcept override {
            if (!renderers.count(emitter.getUniqueType())) {
                renderers.emplace(emitter.getUniqueType(), new EmitterRenderer<SpriteParticle>(emitter));
            }
        }

        void visit(const MeshEmitter& emitter) noexcept override {
            if (!renderers.count(emitter.getUniqueType())) {
                renderers.emplace(emitter.getUniqueType(), new EmitterRenderer<MeshParticle>(emitter));
            }
        }

        void visit(const BeamEmitter& emitter) noexcept override {
            if (!renderers.count(emitter.getUniqueType())) {
                renderers.emplace(emitter.getUniqueType(), new EmitterRenderer<BeamParticle>(emitter));
            }
        }
    };
}