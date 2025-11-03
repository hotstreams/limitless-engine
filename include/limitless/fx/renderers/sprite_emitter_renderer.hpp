#pragma once

#include <limitless/fx/renderers/emitter_renderer.hpp>
#include <limitless/fx/emitters/visitor_collector.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>

#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/vertex_stream/vertex_stream_builder.hpp"
#include <limitless/assets.hpp>
#include <limitless/models/mesh.hpp>

namespace Limitless::fx {
    template<>
    class EmitterRenderer<SpriteParticle> : public AbstractEmitterRenderer {
    private:
        std::shared_ptr<VertexStream> stream;

        const UniqueEmitterShader unique_shader;
    public:
        explicit EmitterRenderer(const SpriteEmitter& emitter)
//            : stream {emitter.getSpawn().max_count * EMITTER_STORAGE_INSTANCE_COUNT, VertexStreamUsage::Dynamic, VertexStreamDraw::Points}
            : stream {VertexStream::builder()
                    .attribute(0, VertexStream::DataType::Vec4, "color", sizeof(SpriteParticle), offsetof(SpriteParticle, color))
                    .attribute(1, VertexStream::DataType::Vec4, "subUV", sizeof(SpriteParticle), offsetof(SpriteParticle, subUV))
                    .attribute(2, VertexStream::DataType::Vec4, "properties", sizeof(SpriteParticle), offsetof(SpriteParticle, properties))
                    .attribute(3, VertexStream::DataType::Vec4, "acceleration", sizeof(SpriteParticle), offsetof(SpriteParticle, acceleration))
                    .attribute(4, VertexStream::DataType::Vec4, "position", sizeof(SpriteParticle), offsetof(SpriteParticle, position))
                    .attribute(5, VertexStream::DataType::Vec4, "rotation", sizeof(SpriteParticle), offsetof(SpriteParticle, rotation))
                    .attribute(6, VertexStream::DataType::Vec4, "velocity", sizeof(SpriteParticle), offsetof(SpriteParticle, velocity))
                    .count(emitter.getSpawn().max_count * EMITTER_STORAGE_INSTANCE_COUNT)
                    .usage(VertexStream::Usage::Dynamic)
                    .draw(VertexStream::Draw::Points)
                    .build()}

            , unique_shader {emitter.getUniqueShaderType()} {
        }

        void update(ParticleCollector<SpriteParticle>& collector) {
            stream->update(collector.yield());
        }

        void draw(Context& ctx,
                  const Assets& assets,
                  ShaderType pass,
                  const ms::Material& material,
                  ms::Blending blending,
                  const UniformSetter& setter) {

            if (material.getBlending() != blending) {
                return;
            }

            auto& shader = assets.shaders.get({unique_shader, pass});

            //TODO: remove from here to somewhere
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
