#pragma once

#include <limitless/fx/renderers/emitter_renderer.hpp>
#include <limitless/fx/emitters/visitor_collector.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>

#include <limitless/core/shader_program.hpp>
#include <limitless/assets.hpp>
#include <limitless/models/mesh.hpp>

namespace Limitless::fx {
    template<>
    class EmitterRenderer<SpriteParticle> : public AbstractEmitterRenderer {
    private:
        VertexStream<SpriteParticle> stream;

        const UniqueEmitterShader unique_shader;
    public:
        explicit EmitterRenderer(const SpriteEmitter& emitter)
            : stream {emitter.getSpawn().max_count * EMITTER_STORAGE_INSTANCE_COUNT, VertexStreamUsage::Dynamic, VertexStreamDraw::Points}
            , unique_shader {emitter.getUniqueShaderType()} {
        }

        void update(ParticleCollector<SpriteParticle>& collector) {
            stream.update(collector.yield());
        }

        void draw(Context& ctx,
                  const Assets& assets,
                  ShaderPass pass,
                  const ms::Material& material,
                  ms::Blending blending,
                  const UniformSetter& setter) {

            if (material.getBlending() != blending) {
                return;
            }

            auto& shader = assets.shaders.get({unique_shader, pass});

            //TODO: remove from here to somewhere
            setBlendingMode(ctx, material.getBlending());
            if (material.getTwoSided()) {
                ctx.disable(Capabilities::CullFace);
            } else {
                ctx.enable(Capabilities::CullFace);
            }

            shader << material;

            setter(shader);

            shader.use();

            stream.draw();
        }
    };
}
