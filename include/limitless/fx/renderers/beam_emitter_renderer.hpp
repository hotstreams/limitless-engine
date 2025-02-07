#pragma once

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>

namespace Limitless::fx {
    template<>
    class EmitterRenderer<BeamParticle> : public AbstractEmitterRenderer {
    private:
        std::shared_ptr<VertexStream> stream;

        const UniqueEmitterShader unique_type;
    public:
        EmitterRenderer(const BeamEmitter& emitter)
            : stream {VertexStream::builder()
                    .attribute(0, VertexStream::DataType::Vec4, "position", sizeof(BeamParticleMapping), offsetof(BeamParticleMapping, position))
                    .attribute(1, VertexStream::DataType::Vec4, "color", sizeof(BeamParticleMapping), offsetof(BeamParticleMapping, color))
                    .attribute(2, VertexStream::DataType::Vec4, "subUV", sizeof(BeamParticleMapping), offsetof(BeamParticleMapping, subUV))
                    .attribute(3, VertexStream::DataType::Vec4, "properties", sizeof(BeamParticleMapping), offsetof(BeamParticleMapping, properties))
                    .attribute(4, VertexStream::DataType::Vec4, "acceleration", sizeof(BeamParticleMapping), offsetof(BeamParticleMapping, acceleration))
                    .attribute(5, VertexStream::DataType::Vec4, "rotation", sizeof(BeamParticleMapping), offsetof(BeamParticleMapping, rotation))
                    .attribute(6, VertexStream::DataType::Vec4, "velocity", sizeof(BeamParticleMapping), offsetof(BeamParticleMapping, velocity))
                    .attribute(7, VertexStream::DataType::Vec4, "uv", sizeof(BeamParticleMapping), offsetof(BeamParticleMapping, uv))
                    .attribute(8, VertexStream::DataType::Vec3, "start", sizeof(BeamParticleMapping), offsetof(BeamParticleMapping, start))
                    .attribute(9, VertexStream::DataType::Vec3, "end", sizeof(BeamParticleMapping), offsetof(BeamParticleMapping, end))
                    .count(emitter.getSpawn().max_count * EMITTER_STORAGE_INSTANCE_COUNT)
                    .usage(VertexStream::Usage::Dynamic)
                    .draw(VertexStream::Draw::Triangles)
                    .build()}

            , unique_type {emitter.getUniqueShaderType()} {
        }

        void update(ParticleCollector<BeamParticleMapping>& collector) {
            stream->update(collector.yield());
        }

        void draw(Context& ctx,
                  const Assets& assets,
                  ShaderType shader_type,
                  const ms::Material& material,
                  ms::Blending blending,
                  const UniformSetter& setter) {

            if (material.getBlending() != blending) {
                return;
            }

            auto& shader = assets.shaders.get({unique_type, shader_type});

            //TODO: move
            setBlendingMode(material.getBlending());
            if (material.getTwoSided()) {
                ctx.disable(Capabilities::CullFace);
            } else {
                ctx.enable(Capabilities::CullFace);
            }

            shader.setMaterial(material);

            setter(shader);

            shader.use();

            stream->draw();
        }
    };
}