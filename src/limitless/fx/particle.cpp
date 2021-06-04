#include <limitless/fx/particle.hpp>

#include <limitless/core/vertex_array.hpp>

using namespace Limitless::fx;
using namespace Limitless;

VertexArray& Limitless::fx::operator<<(VertexArray& vertex_array, const std::pair<SpriteParticle, Buffer&>& attribute) noexcept {
    vertex_array.setAttribute(0, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, color), attribute.second });
    vertex_array.setAttribute(1, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, subUV), attribute.second });
    vertex_array.setAttribute(2, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, properties), attribute.second });
    vertex_array.setAttribute(3, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, acceleration_lifetime), attribute.second });
    vertex_array.setAttribute(4, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, position_size), attribute.second });
    vertex_array.setAttribute(5, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, rotation_time), attribute.second });
    vertex_array.setAttribute(6, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, velocity), attribute.second });
    return vertex_array;
}

VertexArray& Limitless::fx::operator<<(VertexArray& vertex_array, const std::pair<BeamParticleMapping, Buffer&>& attribute) noexcept {
    vertex_array.setAttribute(0, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, position), attribute.second });
    vertex_array.setAttribute(1, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, color), attribute.second });
    vertex_array.setAttribute(2, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, subUV), attribute.second });
    vertex_array.setAttribute(3, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, properties), attribute.second });
    vertex_array.setAttribute(4, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, acceleration_lifetime), attribute.second });
    vertex_array.setAttribute(5, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, rotation_time), attribute.second });
    vertex_array.setAttribute(6, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, velocity_size), attribute.second });
    vertex_array.setAttribute(7, VertexAttribute{4, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, uv_length), attribute.second });
    vertex_array.setAttribute(8, VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, start), attribute.second });
    vertex_array.setAttribute(9, VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, end), attribute.second });
    return vertex_array;
}