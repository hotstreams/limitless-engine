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
        Mesh<SpriteParticle> mesh;

        const ms::Material material;
        const UniqueEmitter unique_type;
    public:
        explicit EmitterRenderer(const SpriteEmitter& emitter)
            : mesh {emitter.getSpawn().max_count * EMITTER_STORAGE_INSTANCE_COUNT,
                    "sprite_emitter",
                    MeshDataType::Dynamic,
                    DrawMode::Points}
            , material {emitter.getMaterial()}
            , unique_type {emitter.getUniqueType()} {
        }

        void update(ParticleCollector<SpriteParticle>& collector) {
            mesh.updateVertices(collector.yield());
        }

        void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending, const UniformSetter& setter) {
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

    inline VertexArray& operator<<(VertexArray& vertex_array, const std::pair<SpriteParticle, Buffer&>& attribute) noexcept {
        vertex_array << VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, position), attribute.second }
                     << VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, color), attribute.second }
                     << VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, rotation), attribute.second }
                     << VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, velocity), attribute.second }
                     << VertexAttribute{1, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, lifetime), attribute.second }
                     << VertexAttribute{1, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, size), attribute.second }
                     << VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, subUV), attribute.second }
                     << VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, properties), attribute.second };
        return vertex_array;
    }
}