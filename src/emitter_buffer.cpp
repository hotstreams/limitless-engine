#include <emitter_buffer.hpp>
#include <shader_storage.hpp>

using namespace GraphicsEngine;

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

SpriteEmitterBuffer::SpriteEmitterBuffer(Emitter& emitter)
    : max_count{emitter.getSpawn().max_count} {
    std::vector<Particle> data(block_size * block_count * max_count);
    buffer = BufferBuilder::buildTriple(Buffer::Type::Array, data, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);

    vertex_array << std::pair<Particle, Buffer&>(Particle{}, *buffer);
}

SpriteEmitterBuffer& SpriteEmitterBuffer::operator--() noexcept {
    --count;
    return *this;
}

SpriteEmitterBuffer& SpriteEmitterBuffer::operator++() {
    ++count;
    if (count > block_count * block_size) {
        std::vector<Particle> data(block_size * ++block_count * max_count);
        buffer = BufferBuilder::buildTriple(Buffer::Type::Array, data, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);
    }
    return *this;
}

void SpriteEmitterBuffer::draw(const UniqueSpriteEmitter& unique_emitter) const {
    vertex_array.bind();

    const auto& shader = shader_storage.get(unique_emitter);

    *shader << *unique_emitter.material;

    shader->use();

    glDrawArrays(GL_POINTS, 0, particles_size);

    buffer->fence();
}

void SpriteEmitterBuffer::updateParticles(const std::vector<Particle>& particles) {
    particles_size = particles.size();
    buffer->mapData(particles.data(), sizeof(Particle) * particles.size());
}

MeshEmitterBuffer::MeshEmitterBuffer(Emitter& emitter)
    : max_count{emitter.getSpawn().max_count} {
    std::vector<MeshParticle> data(block_size * block_count * max_count);
    buffer = BufferBuilder::buildTripleIndexed("mesh_emitter_particles", Buffer::Type::ShaderStorage, data, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);
}

void MeshEmitterBuffer::updateParticles(const std::vector<MeshParticle>& particles) {
    particles_size = particles.size();
    buffer->mapData(particles.data(), sizeof(MeshParticle) * particles.size());
}

void MeshEmitterBuffer::draw(const UniqueMeshEmitter& unique_emitter, const std::shared_ptr<AbstractMesh>& mesh) const {
    const auto& shader = shader_storage.get(unique_emitter);

    *shader << *unique_emitter.unique_sprite_emitter.material;

    shader->use();

    mesh->draw_instanced(particles_size);

    buffer->fence();
}

MeshEmitterBuffer &MeshEmitterBuffer::operator--() noexcept {
    --count;
    return *this;
}

MeshEmitterBuffer &MeshEmitterBuffer::operator++() {
    ++count;
    if (count > block_count * block_size) {
        std::vector<Particle> data(block_size * ++block_count * max_count);
        buffer = BufferBuilder::buildTripleIndexed("mesh_emitter_particles", Buffer::Type::ShaderStorage, data, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);
    }
    return *this;
}
