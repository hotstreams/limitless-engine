#pragma once

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>

namespace Limitless::fx {
    template<>
    class EmitterRenderer<BeamParticle> : public AbstractEmitterRenderer {
    private:
        VertexStream<BeamParticleMapping> stream;

        const UniqueEmitterShader unique_type;
    public:
        EmitterRenderer(const BeamEmitter& emitter)
            : stream {emitter.getSpawn().max_count * EMITTER_STORAGE_INSTANCE_COUNT, VertexStreamUsage::Dynamic, VertexStreamDraw::Triangles}
            , unique_type {emitter.getUniqueShaderType()} {
        }

        void update(ParticleCollector<BeamParticleMapping>& collector) {
            stream.update(collector.yield());
        }

        void draw(Context& ctx,
                  const Assets& assets,
                  ShaderPass shader_type,
                  const ms::Material& material,
                  ms::Blending blending,
                  const UniformSetter& setter) {

            if (material.getBlending() != blending) {
                return;
            }

            auto& shader = assets.shaders.get({unique_type, shader_type});

            //TODO: move
            setBlendingMode(ctx, material.getBlending());
            if (material.getTwoSided()) {
                ctx.disable(Capabilities::CullFace);
            } else {
                ctx.enable(Capabilities::CullFace);
            }

            shader.setMaterial(material);

            setter(shader);

            shader.use();

            stream.draw();
        }
    };
}