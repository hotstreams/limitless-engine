#include <particle_system/emitter_renderer.hpp>

#include <shader_storage.hpp>
#include <core/buffer_builder.hpp>
#include <particle_system/unique_emitter.hpp>
#include <particle_system/emitter.hpp>
#include <iostream>

using namespace GraphicsEngine;

constexpr auto shader_mesh_buffer_name = "mesh_emitter_particles";

VertexArray& GraphicsEngine::operator<<(VertexArray& vertex_array, const std::pair<Particle, Buffer&>& attribute) noexcept {
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

void SpriteEmitterRenderer::checkStorageSize(uint64_t count) {
    if (count > max_particle_count) {
        max_particle_count = count;
        buffer = BufferBuilder::build(Buffer::Type::Array, sizeof(Particle) * max_particle_count, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
    }
}

SpriteEmitterRenderer::SpriteEmitterRenderer(const Emitter& emitter, uint64_t emitter_instance_count)
    : max_particle_count{emitter.getSpawn().max_count * emitter_instance_count} {
    buffer = BufferBuilder::build(Buffer::Type::Array, sizeof(Particle) * max_particle_count, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);

    vertex_array << std::pair<Particle, Buffer&>(Particle{}, *buffer);
}

void SpriteEmitterRenderer::update(SpriteParticleCollector& collector) {
    const auto& particles = collector.yield();
    checkStorageSize(particles.size());
    current_particle_count = particles.size();
    buffer->mapData(particles.data(), sizeof(Particle) * particles.size());
}

void SpriteEmitterRenderer::draw(const UniqueSpriteEmitter& emitter) {
    if (current_particle_count == 0) return;

    vertex_array.bind();

    const auto& shader = shader_storage.get(emitter);

    *shader << *emitter.material;

    shader->use();
    glDrawArrays(GL_POINTS, 0, current_particle_count);
}

void MeshEmitterRenderer::checkStorageSize(uint64_t count) {
    if (count > max_particle_count) {
        max_particle_count = count;
        buffer = BufferBuilder::buildIndexed(shader_mesh_buffer_name, Buffer::Type::ShaderStorage, sizeof(MeshParticle) * max_particle_count, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
    }
}

MeshEmitterRenderer::MeshEmitterRenderer(const Emitter& emitter, uint64_t emitter_instance_count)
    : max_particle_count{emitter.getSpawn().max_count * emitter_instance_count} {
    buffer = BufferBuilder::buildIndexed(shader_mesh_buffer_name, Buffer::Type::ShaderStorage, sizeof(MeshParticle) * max_particle_count, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
}

void MeshEmitterRenderer::update(MeshParticleCollector& collector) {
    const auto& particles = collector.yield();
    checkStorageSize(particles.size());
    current_particle_count = particles.size();
    buffer->mapData(particles.data(), sizeof(MeshParticle) * current_particle_count);
}

void MeshEmitterRenderer::draw(const UniqueMeshEmitter& emitter) {
    if (current_particle_count == 0) return;

    const auto& shader = shader_storage.get(emitter);

    *shader << *emitter.material;

    shader->use();

    emitter.mesh->draw_instanced(current_particle_count);
}
