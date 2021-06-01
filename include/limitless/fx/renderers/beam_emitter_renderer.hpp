#pragma once

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>

namespace Limitless::fx {
    template<>
    class EmitterRenderer<BeamParticle> : public AbstractEmitterRenderer {
    private:
        Mesh<BeamParticleMapping> mesh;

        const UniqueEmitterShader unique_type;
    public:
        EmitterRenderer(const BeamEmitter& emitter)
                : mesh {emitter.getSpawn().max_count * EMITTER_STORAGE_INSTANCE_COUNT,
                        "beam_emitter",
                        MeshDataType::Dynamic,
                        DrawMode::Triangles}
                , unique_type {emitter.getUniqueShaderType()} {
        }

        void update(ParticleCollector<BeamParticleMapping>& collector) {
            mesh.updateVertices(collector.yield());
        }

        void draw(Context& ctx,
                  const Assets& assets,
                  ShaderPass shader_type,
                  const ms::Material& material,
                  ms::Blending blending,
                  const UniformSetter& setter) const {

            if (material.getBlending() != blending) {
                return;
            }

            auto& shader = assets.shaders.get({unique_type, shader_type});

            setBlendingMode(ctx, material.getBlending());
            if (material.getTwoSided()) {
                ctx.disable(Capabilities::CullFace);
            } else {
                ctx.enable(Capabilities::CullFace);
            }

            shader << material;

            setter(shader);

            shader.use();

            mesh.draw();
        }
    };
}