#include <emitter_module.hpp>
#include <emitter.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

using namespace GraphicsEngine;

void EmitterModule::initialize([[maybe_unused]] Emitter& emitter, [[maybe_unused]] Particle& particle) noexcept {

}

void EmitterModule::update([[maybe_unused]] Emitter& emitter, [[maybe_unused]] std::vector<Particle>& particles, [[maybe_unused]] float dt) noexcept {

}

InitialLocation::InitialLocation(Distribution<glm::vec3>* distribution) noexcept
    : distribution{distribution} {

}

InitialLocation::InitialLocation(const InitialLocation& module) noexcept
    : distribution{module.distribution->clone()} {
}

void InitialLocation::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.position += distribution->get();
}

InitialLocation* InitialLocation::clone() const noexcept {
    return new InitialLocation(*this);
}

InitialRotation::InitialRotation(Distribution<glm::vec3>* distribution) noexcept
    : distribution{distribution} {

}

void InitialRotation::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.angle += distribution->get();
}

InitialRotation* InitialRotation::clone() const noexcept {
    return new InitialRotation(*this);
}

InitialRotation::InitialRotation(const InitialRotation& module) noexcept
        : distribution{module.distribution->clone()} {
}

InitialVelocity::InitialVelocity(Distribution<glm::vec3>* distribution) noexcept
        : distribution{distribution} {

}

void InitialVelocity::initialize(Emitter& emitter, Particle& particle) noexcept {
    const auto& rotation = emitter.getLocalRotation() + emitter.getRotation();

    auto rot_matrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    rot_matrix = glm::rotate(rot_matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    rot_matrix = glm::rotate(rot_matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    particle.velocity += glm::vec3(rot_matrix * glm::vec4(distribution->get(), 1.0));
}

InitialVelocity* InitialVelocity::clone() const noexcept {
    return new InitialVelocity(*this);
}

InitialVelocity::InitialVelocity(const InitialVelocity& module) noexcept
        : distribution{module.distribution->clone()} {
}

InitialColor::InitialColor(Distribution<glm::vec4>* distribution) noexcept
        : distribution{distribution} {
}

void InitialColor::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.color += distribution->get();
}

InitialColor* InitialColor::clone() const noexcept {
    return new InitialColor(*this);
}

InitialColor::InitialColor(const InitialColor& module) noexcept
        : distribution{module.distribution->clone()} {
}

InitialSize::InitialSize(Distribution<float>* distribution) noexcept
        : distribution{distribution} {

}

void InitialSize::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.color += distribution->get();
}

InitialSize* InitialSize::clone() const noexcept {
    return new InitialSize(*this);
}

InitialSize::InitialSize(const InitialSize& module) noexcept
        : distribution{module.distribution->clone()} {
}

InitialAcceleration::InitialAcceleration(Distribution<glm::vec3>* distribution) noexcept
        : distribution{distribution} {

}

void InitialAcceleration::initialize(Emitter& emitter, Particle& particle) noexcept {
    const auto& rotation = emitter.getLocalRotation() + emitter.getRotation();

    auto rot_matrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    rot_matrix = glm::rotate(rot_matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    rot_matrix = glm::rotate(rot_matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    particle.acceleration += glm::vec3(rot_matrix * glm::vec4(distribution->get(), 1.0));
}

InitialAcceleration* InitialAcceleration::clone() const noexcept {
    return new InitialAcceleration(*this);
}

SubUV::SubUV(const glm::vec2& tex_size, float fps, const glm::vec2& frame_count) noexcept
    : fps{fps}, subUV_factor{1.0f}, texture_size{tex_size}, frame_count{frame_count} {
    updateFrames();
}

void SubUV::updateFrames() noexcept {
    uint32_t width = texture_size.x / frame_count.x;
    uint32_t height = texture_size.y / frame_count.y;

    frames.clear();
    for (uint32_t i = 0; i < frame_count.x; ++i)
        for (uint32_t j = 0; j < frame_count.y; ++j)
            frames.emplace_back(glm::vec2{
                static_cast<float>(j) * static_cast<float>(width) / texture_size.x,
                static_cast<float>(i) * static_cast<float>(height) / texture_size.y});

    subUV_factor.x = static_cast<float>(width) / texture_size.x;
    subUV_factor.y = static_cast<float>(height) / texture_size.y;
}

void SubUV::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.subUV.x = subUV_factor.x;
    particle.subUV.y = subUV_factor.y;
    particle.subUV.z = frames[0].x;
    particle.subUV.w = frames[0].y;
}

void SubUV::update([[maybe_unused]] Emitter& emitter, std::vector<Particle>& particles, [[maybe_unused]] float dt) noexcept {
    if (first_update) {
        last_time = std::chrono::steady_clock::now();
        first_update = false;
    }

    auto current_time = std::chrono::steady_clock::now();

    if (std::chrono::duration_cast<std::chrono::duration<float>>(current_time - last_time).count() >= (1.0f / fps)) {
        for (auto& p : particles) {

            auto current_frame = glm::vec2{p.subUV.z, p.subUV.w};
            auto it = std::find(frames.begin(), frames.end(), current_frame);

            auto next_frame = (*it == frames.back()) ? frames[0] : *(++it);

            p.subUV.z = next_frame.x;
            p.subUV.w = next_frame.y;
        }

        last_time = current_time;
    }
}

SubUV* SubUV::clone() const noexcept {
    return new SubUV(*this);
}

Lifetime::Lifetime(Distribution<float>* distribution) noexcept
        : distribution{distribution} {

}

void Lifetime::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.lifetime += distribution->get();
}

void Lifetime::update([[maybe_unused]] Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
    for (auto& particle : particles)
        particle.lifetime -= dt;
}

Lifetime* Lifetime::clone() const noexcept {
    return new Lifetime(*this);
}

Lifetime::Lifetime(const Lifetime& module) noexcept
        : distribution{module.distribution->clone()} {
}

InitialAcceleration::InitialAcceleration(const InitialAcceleration& module) noexcept
        : distribution{module.distribution->clone()} {
}

ColorByLife::ColorByLife(Distribution<glm::vec4>* distribution) noexcept
    : distribution{distribution} {

}

ColorByLife::ColorByLife(const ColorByLife& module) noexcept
    : distribution{module.distribution->clone()} {
}

void ColorByLife::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    switch (distribution->getType()) {
        case DistributionType::Const:
            particle.color = distribution->get();
            break;
        case DistributionType::Range:
            particle.color = static_cast<RangeDistribution<glm::vec4>&>(*distribution).getMin();
            break;
        case DistributionType::Curve:
            break;
    }
}

void ColorByLife::update([[maybe_unused]] Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
    switch (distribution->getType()) {
        case DistributionType::Const:
            break;
        case DistributionType::Range: {
            const auto &range = static_cast<RangeDistribution<glm::vec4> &>(*distribution);
            for (auto &particle : particles) {
                auto tick = particle.lifetime / dt;
                auto tick_color = (range.getMax() - particle.color) / tick;
                particle.color += tick_color;
            }
            break;
        }
        case DistributionType::Curve:
            break;
    }
}

ColorByLife* ColorByLife::clone() const noexcept {
    return new ColorByLife(*this);
}

RotationRate::RotationRate(Distribution<glm::vec3>* distribution) noexcept
    : distribution{distribution} {

}

RotationRate::RotationRate(const RotationRate& module) noexcept
    : distribution{module.distribution->clone()} {

}

void RotationRate::update([[maybe_unused]] Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
    for (auto& particle : particles) {
        particle.angle += distribution->get() * dt;
    }
}

RotationRate* RotationRate::clone() const noexcept {
    return new RotationRate(*this);
}

SizeByLife::SizeByLife(Distribution<float>* distribution, float factor) noexcept
        : distribution{distribution}, factor{factor} {
}

SizeByLife::SizeByLife(const SizeByLife& module) noexcept
        : distribution{module.distribution->clone()}, factor{module.factor} {
}

void SizeByLife::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    switch (distribution->getType()) {
        case DistributionType::Const:
            particle.size = distribution->get();
            break;
        case DistributionType::Range:
            particle.size = static_cast<RangeDistribution<float>&>(*distribution).getMin();
            break;
        case DistributionType::Curve:
            break;
    }
}

void SizeByLife::update([[maybe_unused]] Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
    switch (distribution->getType()) {
        case DistributionType::Const:
            break;
        case DistributionType::Range: {
            const auto &range = static_cast<RangeDistribution<float> &>(*distribution);
            for (auto& particle : particles) {
                auto tick = particle.size / dt;
                auto tick_size = factor < 0 ? particle.size - range.getMin() : range.getMax() - particle.size;
                particle.size += tick_size / tick;
            }
            break;
        }
        case DistributionType::Curve:
            break;
    }
}

SizeByLife* SizeByLife::clone() const noexcept {
    return new SizeByLife(*this);
}

//VelocityByLife::VelocityByLife(Distribution<glm::vec3>* distribution, float factor) noexcept
//        : distribution{distribution}, factor{factor} {
//}
//
//VelocityByLife::VelocityByLife(const VelocityByLife& module) noexcept
//        : distribution{module.distribution->clone()}, factor{module.factor} {
//}
//
//void VelocityByLife::update(Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
//    switch (distribution->getType()) {
//        case DistributionType::Const:
//            break;
//        case DistributionType::Range:
//            particle.size = static_cast<RangeDistribution<float>&>(*distribution).getMin();
//            break;
//        case DistributionType::Curve:
//            break;
//    }
//}

MeshLocation::MeshLocation(std::shared_ptr<AbstractMesh> mesh) noexcept
    : mesh{std::move(mesh)}, distribution(0.0f, 1.0f) {
}

glm::vec3 MeshLocation::getPositionOnTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) noexcept {
    auto r1 = std::sqrt(distribution(generator));
    auto r2 = distribution(generator);
    auto m1 = 1.0f - r1;
    auto m2 = r1 * (1.0f - r2);
    auto m3 = r2 * r1;

    return (m1 * a) + (m2 * b) + (m3 * c);
}

glm::vec3 MeshLocation::getPositionOnMesh() noexcept {
    const auto& vertices = static_cast<Mesh<VertexNormalTangent>&>(*mesh).getVertices();

    auto distr = std::uniform_int_distribution(0ULL, vertices.size() - 3);
    const auto index = distr(generator);
    return getPositionOnTriangle(vertices[index].position,
                                 vertices[index + 1].position,
                                 vertices[index + 2].position);
}

void MeshLocation::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.position += getPositionOnMesh();
}

void MeshLocation::update([[maybe_unused]] Emitter& emitter, [[maybe_unused]] std::vector<Particle>& particles, [[maybe_unused]] float dt) noexcept {
    //TODO: skeletal mesh update
}

MeshLocation* MeshLocation::clone() const noexcept {
    return new MeshLocation(*this);
}
