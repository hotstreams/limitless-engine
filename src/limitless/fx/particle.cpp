#include <limitless/fx/particle.hpp>

#include <limitless/core/vertex_array.hpp>

using namespace Limitless::fx;
using namespace Limitless;

VertexArray& Limitless::fx::operator<<(VertexArray& vertex_array, const std::pair<SpriteParticle, const std::shared_ptr<Buffer>&>& attribute) noexcept {
    vertex_array.setAttribute<glm::vec4>(0, false, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, color), attribute.second);
    vertex_array.setAttribute<glm::vec4>(1, false, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, subUV), attribute.second);
    vertex_array.setAttribute<glm::vec4>(2, false, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, properties), attribute.second);
    vertex_array.setAttribute<glm::vec4>(3, false, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, acceleration), attribute.second);
    vertex_array.setAttribute<glm::vec4>(4, false, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, position), attribute.second);
    vertex_array.setAttribute<glm::vec4>(5, false, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, rotation), attribute.second);
    vertex_array.setAttribute<glm::vec4>(6, false, sizeof(SpriteParticle), (GLvoid*)offsetof(SpriteParticle, velocity), attribute.second);
    return vertex_array;
}

VertexArray& Limitless::fx::operator<<(VertexArray& vertex_array, const std::pair<BeamParticleMapping, const std::shared_ptr<Buffer>&>& attribute) noexcept {
    vertex_array.setAttribute<glm::vec4>(0, false, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, position), attribute.second);
    vertex_array.setAttribute<glm::vec4>(1, false, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, color), attribute.second);
    vertex_array.setAttribute<glm::vec4>(2, false, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, subUV), attribute.second);
    vertex_array.setAttribute<glm::vec4>(3, false, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, properties), attribute.second);
    vertex_array.setAttribute<glm::vec4>(4, false, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, acceleration), attribute.second);
    vertex_array.setAttribute<glm::vec4>(5, false, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, rotation), attribute.second);
    vertex_array.setAttribute<glm::vec4>(6, false, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, velocity), attribute.second);
    vertex_array.setAttribute<glm::vec4>(7, false, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, uv), attribute.second);
    vertex_array.setAttribute<glm::vec3>(8, false, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, start), attribute.second);
    vertex_array.setAttribute<glm::vec3>(9, false, sizeof(BeamParticleMapping), (GLvoid*)offsetof(BeamParticleMapping, end), attribute.second);
    return vertex_array;
}