#pragma once

#include <limitless/fx/emitters/emitter_visitor.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class ParticleCollector : public EmitterVisitor {
    private:
        std::vector<Particle> particles;
        const UniqueEmitterRenderer& emitter_type;
    public:
        explicit ParticleCollector(const UniqueEmitterRenderer& _emitter_type) noexcept
            : emitter_type {_emitter_type} {}
        ~ParticleCollector() override = default;

        void visit(const SpriteEmitter& emitter) noexcept override {
            if constexpr (std::is_same_v<Particle, SpriteParticle>) {
                if (emitter_type == emitter.getUniqueRendererType()) {
                    particles.insert(particles.end(), emitter.getParticles().begin(), emitter.getParticles().end());
                }
            }
        }

        void visit(const MeshEmitter& emitter) noexcept override {
            if constexpr (std::is_same_v<Particle, MeshParticle>) {
                if (emitter_type == emitter.getUniqueRendererType()) {
                    particles.insert(particles.end(), emitter.getParticles().begin(), emitter.getParticles().end());
                }
            }
        }

        void visit(const BeamEmitter& emitter) noexcept override {
            if constexpr (std::is_same_v<Particle, BeamParticleMapping>) {
                if (emitter_type == emitter.getUniqueRendererType()) {
                    particles.insert(particles.end(), emitter.getParticles().begin(), emitter.getParticles().end());
                }
            }
        }

        [[nodiscard]] auto&& yield() { return std::move(particles); }
    };
}