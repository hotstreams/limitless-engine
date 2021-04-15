#include <limitless/particle_system/emitter_module.hpp>
#include <limitless/particle_system/emitter.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

using namespace LimitlessEngine;

EmitterModule::EmitterModule(EmitterModuleType _type) noexcept
    : type {_type} {
}

void EmitterModule::initialize([[maybe_unused]] Emitter& emitter, [[maybe_unused]] Particle& particle) noexcept {
}

void EmitterModule::update([[maybe_unused]] Emitter& emitter, [[maybe_unused]] std::vector<Particle>& particles, [[maybe_unused]] float dt) noexcept {
}

InitialLocation::InitialLocation(std::unique_ptr<Distribution<glm::vec3>> _distribution) noexcept
    : EmitterModule(EmitterModuleType::InitialLocation)
    , distribution{std::move(_distribution)} {
}

InitialLocation::InitialLocation(const InitialLocation& module) noexcept
    : EmitterModule(EmitterModuleType::InitialLocation)
    , distribution{module.distribution->clone()} {
}

void InitialLocation::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.position += distribution->get();
}

InitialLocation* InitialLocation::clone() const noexcept {
    return new InitialLocation(*this);
}

InitialRotation::InitialRotation(std::unique_ptr<Distribution<glm::vec3>> _distribution) noexcept
    : EmitterModule(EmitterModuleType::InitialRotation)
    , distribution{std::move(_distribution)} {

}

void InitialRotation::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    const auto rot = emitter.getRotation() * emitter.getLocalRotation();
    particle.rotation += distribution->get() * rot;
}

InitialRotation* InitialRotation::clone() const noexcept {
    return new InitialRotation(*this);
}

InitialRotation::InitialRotation(const InitialRotation& module) noexcept
    : EmitterModule(EmitterModuleType::InitialRotation)
    , distribution{module.distribution->clone()} {
}

InitialVelocity::InitialVelocity(std::unique_ptr<Distribution<glm::vec3>> _distribution) noexcept
    : EmitterModule(EmitterModuleType::InitialVelocity)
    , distribution{std::move(_distribution)} {
}

void InitialVelocity::initialize(Emitter& emitter, Particle& particle) noexcept {
    const auto rot = emitter.getRotation() * emitter.getLocalRotation();
    particle.velocity = distribution->get() * rot;
}

InitialVelocity* InitialVelocity::clone() const noexcept {
    return new InitialVelocity(*this);
}

InitialVelocity::InitialVelocity(const InitialVelocity& module) noexcept
    : EmitterModule(EmitterModuleType::InitialVelocity)
    , distribution{module.distribution->clone()} {
}

InitialColor::InitialColor(std::unique_ptr<Distribution<glm::vec4>> _distribution) noexcept
    : EmitterModule(EmitterModuleType::InitialColor)
    , distribution{std::move(_distribution)} {
}

void InitialColor::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.color += distribution->get();
}

InitialColor* InitialColor::clone() const noexcept {
    return new InitialColor(*this);
}

InitialColor::InitialColor(const InitialColor& module) noexcept
    : EmitterModule(EmitterModuleType::InitialColor)
    , distribution{module.distribution->clone()} {
}

InitialSize::InitialSize(std::unique_ptr<Distribution<float>> _distribution) noexcept
    : EmitterModule(EmitterModuleType::InitialSize)
    , distribution{std::move(_distribution)} {
}

void InitialSize::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.size = distribution->get();
}

InitialSize* InitialSize::clone() const noexcept {
    return new InitialSize(*this);
}

InitialSize::InitialSize(const InitialSize& module) noexcept
    : EmitterModule(EmitterModuleType::InitialSize)
    , distribution{module.distribution->clone()} {
}

InitialAcceleration::InitialAcceleration(std::unique_ptr<Distribution<glm::vec3>> _distribution) noexcept
    : EmitterModule(EmitterModuleType::InitialAcceleration)
    , distribution{std::move(_distribution)} {
}

void InitialAcceleration::initialize(Emitter& emitter, Particle& particle) noexcept {
    const auto rot = emitter.getRotation() * emitter.getLocalRotation();
    particle.acceleration = distribution->get() * rot;
}

InitialAcceleration* InitialAcceleration::clone() const noexcept {
    return new InitialAcceleration(*this);
}

SubUV::SubUV(const glm::vec2& tex_size, float _fps, const glm::vec2& _frame_count) noexcept
    : EmitterModule(EmitterModuleType::SubUV)
    , fps {_fps}
    , subUV_factor {1.0f}
    , texture_size {tex_size}
    , frame_count {_frame_count} {
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

Lifetime::Lifetime(std::unique_ptr<Distribution<float>> _distribution) noexcept
    : EmitterModule(EmitterModuleType::Lifetime)
    , distribution{std::move(_distribution)} {
}

void Lifetime::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    particle.lifetime = distribution->get();
}

void Lifetime::update([[maybe_unused]] Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
    for (auto& particle : particles) {
        particle.lifetime -= dt;
    }
}

Lifetime* Lifetime::clone() const noexcept {
    return new Lifetime(*this);
}

Lifetime::Lifetime(const Lifetime& module) noexcept
    : EmitterModule(EmitterModuleType::Lifetime)
    , distribution{module.distribution->clone()} {
}

InitialAcceleration::InitialAcceleration(const InitialAcceleration& module) noexcept
    : EmitterModule(EmitterModuleType::InitialAcceleration)
    , distribution{module.distribution->clone()} {
}

ColorByLife::ColorByLife(std::unique_ptr<Distribution<glm::vec4>> _distribution) noexcept
    : EmitterModule(EmitterModuleType::ColorByLife)
    , distribution{std::move(_distribution)} {
}

ColorByLife::ColorByLife(const ColorByLife& module) noexcept
    : EmitterModule(EmitterModuleType::ColorByLife)
    , distribution{module.distribution->clone()} {
}

void ColorByLife::update([[maybe_unused]] Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
    for (auto& particle : particles) {
        const auto tick = particle.lifetime / dt;
        const auto tick_color = glm::abs(distribution->get() - particle.color) / tick;
        particle.color += tick_color;
    }
}

ColorByLife* ColorByLife::clone() const noexcept {
    return new ColorByLife(*this);
}

RotationRate::RotationRate(std::unique_ptr<Distribution<glm::vec3>> _distribution) noexcept
    : EmitterModule(EmitterModuleType::RotationRate)
    , distribution{std::move(_distribution)} {
}

RotationRate::RotationRate(const RotationRate& module) noexcept
    : EmitterModule(EmitterModuleType::RotationRate)
    , distribution{module.distribution->clone()} {
}

void RotationRate::update([[maybe_unused]] Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
    const auto rot = emitter.getRotation() * emitter.getLocalRotation();
    for (auto& particle : particles) {
        particle.rotation += (distribution->get() * rot) * dt;
    }
}

RotationRate* RotationRate::clone() const noexcept {
    return new RotationRate(*this);
}

SizeByLife::SizeByLife(std::unique_ptr<Distribution<float>> _distribution, float _factor) noexcept
    : EmitterModule(EmitterModuleType::SizeByLife)
    , distribution{std::move(_distribution)}, factor{_factor} {
}

SizeByLife::SizeByLife(const SizeByLife& module) noexcept
    : EmitterModule(EmitterModuleType::SizeByLife)
    , distribution{module.distribution->clone()}
    , factor{module.factor} {
}

void SizeByLife::update([[maybe_unused]] Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
    for (auto& particle : particles) {
        const auto tick = particle.lifetime / dt;
        const auto tick_size = glm::abs(distribution->get() - particle.size);
        particle.size += tick_size * factor / tick;
        particle.size = std::clamp(particle.size, MIN_SIZE, MAX_SIZE);
    }
}

SizeByLife* SizeByLife::clone() const noexcept {
    return new SizeByLife(*this);
}

VelocityByLife::VelocityByLife(std::unique_ptr<Distribution<glm::vec3>> _distribution) noexcept
    : EmitterModule(EmitterModuleType::VelocityByLife)
    , distribution{std::move(_distribution)} {
}

VelocityByLife::VelocityByLife(const VelocityByLife& module) noexcept
    : EmitterModule(EmitterModuleType::VelocityByLife)
    , distribution{module.distribution->clone()} {
}

void VelocityByLife::update(Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
    const auto rot = emitter.getRotation() * emitter.getLocalRotation();
    for (auto& particle : particles) {
        const auto tick = particle.lifetime / dt;
        const auto tick_vel = (rot * distribution->get() - particle.velocity) / tick;
        particle.velocity += tick_vel;
    }
}

VelocityByLife* VelocityByLife::clone() const noexcept {
    return new VelocityByLife(*this);
}

MeshLocation::MeshLocation(std::shared_ptr<AbstractMesh> _mesh) noexcept
    : EmitterModule(EmitterModuleType::MeshLocation)
    , mesh{std::move(_mesh)}
    , distribution(0.0f, 1.0f) {
}

glm::vec3 MeshLocation::getPositionOnTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) noexcept {
    const auto r1 = std::sqrt(distribution(generator));
    const auto r2 = distribution(generator);
    const auto m1 = 1.0f - r1;
    const auto m2 = r1 * (1.0f - r2);
    const auto m3 = r2 * r1;

    return (m1 * a) + (m2 * b) + (m3 * c);
}

glm::vec3 MeshLocation::getPositionOnMesh() noexcept {
    const auto& vertices = static_cast<Mesh<VertexNormalTangent>&>(*mesh).getVertices();

    using vector_size_type = std::remove_reference_t<decltype(vertices)>::size_type;
    auto distr = std::uniform_int_distribution(static_cast<vector_size_type>(0), vertices.size() - 3);

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

CustomMaterialModule::CustomMaterialModule(std::unique_ptr<Distribution<float>> prop1,
                                           std::unique_ptr<Distribution<float>> prop2,
                                           std::unique_ptr<Distribution<float>> prop3,
                                           std::unique_ptr<Distribution<float>> prop4) noexcept
    : EmitterModule(EmitterModuleType::CustomMaterial)
    , properties{std::move(prop1),
                 std::move(prop2),
                 std::move(prop3),
                 std::move(prop4)} {

}

CustomMaterialModule::CustomMaterialModule(const CustomMaterialModule& rhs)
        : EmitterModule(EmitterModuleType::CustomMaterial) {
    for (uint32_t i = 0; i < rhs.properties.size(); ++i) {
        if (rhs.properties[i]) {
            properties[i] = std::unique_ptr<Distribution<float>>(rhs.properties[i]->clone());
        }
    }
}

void CustomMaterialModule::initialize([[maybe_unused]] Emitter& emitter, Particle& particle) noexcept {
    for (uint32_t i = 0; i < properties.size(); i++) {
        if (properties[i]) {
            particle.properties[i] = properties[i]->get();
        }
    }
}

CustomMaterialModule* CustomMaterialModule::clone() const noexcept {
    return new CustomMaterialModule(*this);
}

CustomMaterialModuleByLife::CustomMaterialModuleByLife(std::unique_ptr<Distribution<float>> prop1,
                                                       std::unique_ptr<Distribution<float>> prop2,
                                                       std::unique_ptr<Distribution<float>> prop3,
                                                       std::unique_ptr<Distribution<float>> prop4) noexcept
        : EmitterModule(EmitterModuleType::CustomMaterialByLife)
        , properties{std::move(prop1),
                     std::move(prop2),
                     std::move(prop3),
                     std::move(prop4)} {
}

CustomMaterialModuleByLife::CustomMaterialModuleByLife(const CustomMaterialModuleByLife& rhs)
    : EmitterModule(EmitterModuleType::CustomMaterialByLife) {
    for (uint32_t i = 0; i < rhs.properties.size(); ++i) {
        if (rhs.properties[i]) {
            properties[i] = std::unique_ptr<Distribution<float>>(rhs.properties[i]->clone());
        }
    }
}

CustomMaterialModuleByLife *CustomMaterialModuleByLife::clone() const noexcept {
    return new CustomMaterialModuleByLife(*this);
}

void CustomMaterialModuleByLife::update([[maybe_unused]] Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept {
    for (auto& particle : particles) {
        for (uint32_t i = 0; i < properties.size(); ++i) {
            if (properties[i]) {
                const auto tick = particle.lifetime / dt;
                particle.properties[i] += (properties[i]->get() - particle.properties[i]) / tick;
            }
        }
    }
}
