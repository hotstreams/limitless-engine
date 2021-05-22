#pragma once

#include <limitless/fx/renderers/emitter_renderer.hpp>
#include <limitless/fx/emitters/visitor_collector.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>

#include <limitless/core/shader_program.hpp>
#include <limitless/assets.hpp>
#include <limitless/models/mesh.hpp>

namespace Limitless::fx {
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

    template<>
    class EmitterRenderer<SpriteParticle> : public AbstractEmitterRenderer {
    private:
        Mesh<SpriteParticle> mesh;

        const UniqueEmitterShader unique_shader;
    public:
        explicit EmitterRenderer(const SpriteEmitter& emitter)
                : mesh {emitter.getSpawn().max_count, "sprite_emitter", MeshDataType::Dynamic, DrawMode::Points}
                , unique_shader {emitter.getUniqueShaderType()} {
        }

        void update(ParticleCollector<SpriteParticle>& collector) {
            mesh.updateVertices(collector.yield());
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

            setBlendingMode(ctx, material.getBlending());

            shader << material;

            setter(shader);

            shader.use();

            mesh.draw();
        }
    };
}