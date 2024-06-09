#pragma once

#include <limitless/fx/renderers/emitter_renderer.hpp>

namespace Limitless::fx {
    template<>
    class EmitterRenderer<MeshParticle> : public AbstractEmitterRenderer {
    private:
        std::shared_ptr<Buffer> buffer;
        uint64_t max_particle_count {};

        uint64_t current_particle_count {};

        const UniqueEmitterShader unique_type;

        void checkStorageSize(uint64_t count) {
            if (count > max_particle_count) {
                max_particle_count = count;
                buffer = Buffer::builder()
                        .target(Buffer::Type::ShaderStorage)
                        .usage(Buffer::Usage::DynamicDraw)
                        .access(Buffer::MutableAccess::WriteOrphaning)
                        .size(sizeof(MeshParticle) * max_particle_count)
                        .build();
            }
        }

        static constexpr auto SHADER_MESH_BUFFER_NAME = "mesh_emitter_particles";
    public:
        explicit EmitterRenderer(const MeshEmitter& emitter)
            : max_particle_count {emitter.getSpawn().max_count * EMITTER_STORAGE_INSTANCE_COUNT}
            , unique_type {emitter.getUniqueShaderType()} {
            buffer = Buffer::builder()
                    .target(Buffer::Type::ShaderStorage)
                    .usage(Buffer::Usage::DynamicDraw)
                    .access(Buffer::MutableAccess::WriteOrphaning)
                    .size(sizeof(MeshParticle) * max_particle_count)
                    .build();
        }

        void update(ParticleCollector<MeshParticle>& collector) {
            const auto& particles = collector.yield();
            checkStorageSize(particles.size());
            current_particle_count = particles.size();
            buffer->mapData(particles.data(), sizeof(MeshParticle) * current_particle_count);
        }

        void draw(Context& ctx,
                  const Assets& assets,
                  ShaderType pass,
                  const std::shared_ptr<AbstractMesh>& mesh,
                  const ms::Material& material,
                  ms::Blending blending,
                  const UniformSetter& setter) const {
            if (current_particle_count == 0 || material.getBlending() != blending) {
                return;
            }

            auto& shader = assets.shaders.get({unique_type, pass});

            setBlendingMode(material.getBlending());
            if (material.getTwoSided()) {
                ctx.disable(Capabilities::CullFace);
            } else {
                ctx.enable(Capabilities::CullFace);
            }

            shader.setMaterial(material);

            setter(shader);

            Context::apply([this] (Context& ctx) {
                buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, SHADER_MESH_BUFFER_NAME));
            });

            shader.use();

            mesh->draw_instanced(current_particle_count);
        }
    };
}