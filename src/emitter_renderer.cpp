#include <emitter_renderer.hpp>

#include <shader_storage.hpp>
#include <buffer_builder.hpp>
#include <unique_emitter.hpp>
#include <emitter.hpp>

using namespace GraphicsEngine;

constexpr auto shader_mesh_buffer_name = "mesh_emitter_particles";

VertexArray& GraphicsEngine::operator<<(VertexArray& vertex_array, const std::pair<Particle, Buffer&>& attribute) noexcept {
    vertex_array << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr, attribute.second }
                 << VertexAttribute{ 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, color), attribute.second }
                 << VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, angle), attribute.second }
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
        std::vector<Particle> data(count);
        buffer = BufferBuilder::buildTriple(Buffer::Type::Array, data, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);
    }
}

SpriteEmitterRenderer::SpriteEmitterRenderer(const Emitter& emitter, uint64_t emitter_instance_count)
    : max_particle_count{emitter.getSpawn().max_count * emitter_instance_count} {
    std::vector<Particle> data(max_particle_count);
    buffer = BufferBuilder::buildTriple(Buffer::Type::Array, data, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);

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

    buffer->fence();
}

void MeshEmitterRenderer::checkStorageSize(uint64_t count) {
    if (count > max_particle_count) {
        max_particle_count = count;
        std::vector<MeshParticle> data(count);
        buffer = BufferBuilder::buildTripleIndexed(shader_mesh_buffer_name, Buffer::Type::ShaderStorage, data, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);
    }
}

MeshEmitterRenderer::MeshEmitterRenderer(const Emitter& emitter, uint64_t emitter_instance_count)
    : max_particle_count{emitter.getSpawn().max_count * emitter_instance_count} {
    std::vector<MeshParticle> data(max_particle_count);
    buffer = BufferBuilder::buildTripleIndexed(shader_mesh_buffer_name, Buffer::Type::ShaderStorage, data, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);
}

void MeshEmitterRenderer::update(MeshParticleCollector& collector) {
    const auto& particles = collector.yield();
    checkStorageSize(particles.size());
    current_particle_count = particles.size();
    buffer->mapData(particles.data(), sizeof(MeshParticle) * particles.size());
}

void MeshEmitterRenderer::draw(const UniqueMeshEmitter& emitter) {
    if (current_particle_count == 0) return;

    const auto& shader = shader_storage.get(emitter);

    *shader << *emitter.material;

    shader->use();

    emitter.mesh->draw_instanced(current_particle_count);

    buffer->fence();
}
