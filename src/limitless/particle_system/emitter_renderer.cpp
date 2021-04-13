#include <limitless/particle_system/emitter_renderer.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/buffer_builder.hpp>
#include <limitless/particle_system/unique_emitter.hpp>
#include <limitless/particle_system/emitter.hpp>
#include <limitless/material_system/material.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <limitless/assets.hpp>

using namespace LimitlessEngine;

constexpr auto shader_mesh_buffer_name = "mesh_emitter_particles";

VertexArray& LimitlessEngine::operator<<(VertexArray& vertex_array, const std::pair<Particle, Buffer&>& attribute) noexcept {
    vertex_array << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, position), attribute.second }
                 << VertexAttribute{ 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, color), attribute.second }
                 << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, rotation), attribute.second }
                 << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, velocity), attribute.second }
                 << VertexAttribute{ 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, lifetime), attribute.second }
                 << VertexAttribute{ 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, size), attribute.second }
                 << VertexAttribute{ 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, subUV), attribute.second }
                 << VertexAttribute{ 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, properties), attribute.second };
    return vertex_array;
}

SpriteEmitterRenderer::SpriteEmitterRenderer(const Emitter& emitter, uint64_t emitter_instance_count)
    : mesh {emitter.getSpawn().max_count * emitter_instance_count, "sprite_emitter", MeshDataType::Dynamic, DrawMode::Points} {
}

void SpriteEmitterRenderer::update(SpriteParticleCollector& collector) {
    mesh.updateVertices(collector.yield());
}

void SpriteEmitterRenderer::draw(const Assets& assets, MaterialShader shader_type, const UniqueSpriteEmitter& emitter, const UniformSetter& setter) {
    auto& shader = assets.shaders.get(shader_type, emitter);

    setBlendingMode(emitter.material->getBlending());

    shader << *emitter.material;

    setter(shader);

    shader.use();

    mesh.draw();
}

void MeshEmitterRenderer::checkStorageSize(uint64_t count) {
    if (count > max_particle_count) {
        max_particle_count = count;
        BufferBuilder builder;
        buffer = builder .setTarget(Buffer::Type::ShaderStorage)
                         .setUsage(Buffer::Usage::DynamicDraw)
                         .setAccess(Buffer::MutableAccess::WriteOrphaning)
                         .setDataSize(sizeof(MeshParticle) * max_particle_count)
                         .build();
    }
}

MeshEmitterRenderer::MeshEmitterRenderer(const Emitter& emitter, uint64_t emitter_instance_count)
    : max_particle_count{emitter.getSpawn().max_count * emitter_instance_count} {
    BufferBuilder builder;
    buffer = builder .setTarget(Buffer::Type::ShaderStorage)
            .setUsage(Buffer::Usage::DynamicDraw)
            .setAccess(Buffer::MutableAccess::WriteOrphaning)
            .setDataSize(sizeof(MeshParticle) * max_particle_count)
            .build();
}

void MeshEmitterRenderer::update(MeshParticleCollector& collector) {
    const auto& particles = collector.yield();
    checkStorageSize(particles.size());
    current_particle_count = particles.size();
    buffer->mapData(particles.data(), sizeof(MeshParticle) * current_particle_count);
}

void MeshEmitterRenderer::draw(const Assets& assets, const UniqueMeshEmitter& emitter) const {
    if (current_particle_count == 0) return;

    auto& shader = assets.shaders.get(emitter);

    setBlendingMode(emitter.material->getBlending());

    shader << *emitter.material;

    buffer->bindBase(ContextState::getState(glfwGetCurrentContext())->getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, shader_mesh_buffer_name));

    shader.use();

    emitter.mesh->draw_instanced(current_particle_count);
}
