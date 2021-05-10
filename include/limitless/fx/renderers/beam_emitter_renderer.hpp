#pragma once

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>

namespace Limitless::fx {
    template<>
    class EmitterRenderer<BeamParticle> : public AbstractEmitterRenderer {
    private:
        Mesh<BeamParticleMapping> mesh;

        const ms::Material material;
        const UniqueEmitter unique_type;
    public:
        EmitterRenderer(const BeamEmitter& emitter)
                : mesh {emitter.getSpawn().max_count * EMITTER_STORAGE_INSTANCE_COUNT,
                        "beam_emitter",
                        MeshDataType::Dynamic,
                        DrawMode::Triangles}
                , material {emitter.getMaterial()}
                , unique_type {emitter.getUniqueType()} {
        }

        void update(ParticleCollector<BeamParticleMapping>& collector) {
            mesh.updateVertices(collector.yield());
        }

        void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending, const UniformSetter& setter) const {
            if (material.getBlending() != blending) {
                return;
            }

            auto& shader = assets.shaders.get({unique_type, shader_type});

            setBlendingMode(ctx, material.getBlending());

            shader << material;

            setter(shader);

            shader.use();

            mesh.draw();
        }
    };

    inline VertexArray& operator<<(VertexArray& vertex_array, const std::pair<BeamParticleMapping, Buffer&>& attribute) noexcept {
        vertex_array.setAttribute(0, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, position), attribute.second });
        vertex_array.setAttribute(1, VertexAttribute{2, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, uv), attribute.second });
        vertex_array.setAttribute(2, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, color), attribute.second });
        vertex_array.setAttribute(3, VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, rotation), attribute.second });
        vertex_array.setAttribute(4, VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, velocity), attribute.second });
        vertex_array.setAttribute(5, VertexAttribute{1, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, lifetime), attribute.second });
        vertex_array.setAttribute(6, VertexAttribute{1, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, size), attribute.second });
        vertex_array.setAttribute(7, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, subUV), attribute.second });
        vertex_array.setAttribute(8, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, properties), attribute.second });
        return vertex_array;
    }
}