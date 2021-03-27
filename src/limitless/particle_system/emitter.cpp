#include <limitless/particle_system/emitter.hpp>

using namespace LimitlessEngine;

Emitter::Emitter(EmitterType type) noexcept
    : type{type}, local_position{0.0f}, position{0.0f}, local_rotation{}, rotation{}, local_space{false} {

}

void Emitter::emit(uint32_t count) noexcept {
    for (uint32_t i = 0; i < count; ++i) {
        Particle particle;

        particle.position = local_position + position;
        particle.rotation = glm::eulerAngles(rotation * local_rotation);

        for (const auto& [type, module] : modules) {
            module->initialize(*this, particle);
        }

        particles.emplace_back(particle);
    }
}

void Emitter::setPosition(const glm::vec3& new_position) noexcept {
    if (local_space) {
        auto diff = new_position + local_position - position;

        for (auto& particle : particles) {
            particle.position += diff;
        }
    }

    position = local_position + new_position;
}

void Emitter::setRotation([[maybe_unused]] const glm::quat& new_rotation) noexcept {
    //TODO: check this
    if (local_space) {
        auto diff = new_rotation + local_rotation - rotation;

        for (auto& particle : particles) {
            particle.rotation += glm::eulerAngles(diff);
        }
    }

    rotation = new_rotation;
}

void Emitter::spawnParticles() noexcept {
    if (spawn.spawn_rate <= 0.0f) {
        return;
    }

    auto current_time = std::chrono::steady_clock::now();

    switch (spawn.mode) {
        case EmitterSpawn::Mode::Spray:
            if ((current_time - spawn.last_spawn).count() >= (1.0f / spawn.spawn_rate)) {
                if (particles.size() < spawn.max_count) {
                    emit(1);
                }
                spawn.last_spawn = current_time;
            }
            break;
        case EmitterSpawn::Mode::Burst:
            if (spawn.loops != spawn.loops_done) {
                if ((current_time - spawn.last_spawn).count() >= (1.0f / spawn.spawn_rate)) {
                    auto emit_count = spawn.burst_count->get();
                    emit_count = (particles.size() + emit_count > spawn.max_count) ? spawn.max_count - particles.size() : emit_count;
                    emit(emit_count);

                    if (spawn.loops != -1) {
                        ++spawn.loops_done;
                    }

                    spawn.last_spawn = current_time;
                }
            }
            break;
    }
}

void Emitter::killParticles() noexcept {
    for (auto it = particles.begin(); it != particles.end();) {
        if (it->lifetime <= 0.0f) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void Emitter::update() {
    using namespace std::chrono;

    if (done) {
        return;
    }

    if (first_update) {
        last_time = steady_clock::now();
        start_time = last_time;
        first_update = false;
    }

    auto current_time = steady_clock::now();
    auto delta_time = duration_cast<std::chrono::duration<float>>(current_time - last_time);
    last_time = current_time;

    spawnParticles();
    killParticles();

    for (auto& [type, module] : modules) {
        module->update(*this, particles, delta_time.count());
    }

    for (auto& particle : particles) {
        particle.position += particle.velocity * delta_time.count();
        particle.velocity += particle.acceleration * delta_time.count();
    }

    if (duration.count() != 0.0f) {
        if (current_time - start_time >= duration) {
            done = true;
        }
    }
}

Emitter* Emitter::clone() const noexcept {
    return nullptr;
}

Emitter::Emitter(const Emitter& emitter) noexcept
    : type{EmitterType::Sprite}, local_position{emitter.local_position}, position{emitter.position},
      local_rotation{emitter.local_rotation}, rotation{emitter.rotation},
      local_space{emitter.local_space}, spawn{emitter.spawn}, duration{emitter.duration} {
    for (const auto& [type, module] : emitter.modules) {
        modules.emplace(type, module->clone());
    }

    particles.reserve(spawn.max_count);
}

Emitter& Emitter::operator=(const Emitter& emitter) noexcept {
    auto copied = Emitter{emitter};
    swap(*this, copied);
    return *this;
}

void LimitlessEngine::swap(Emitter& lhs, Emitter& rhs) noexcept {
    using std::swap;

    swap(lhs.type, rhs.type);
    swap(lhs.local_position, rhs.local_position);
    swap(lhs.position, rhs.position);
    swap(lhs.local_rotation, rhs.local_rotation);
    swap(lhs.rotation, rhs.rotation);
    swap(lhs.local_space, rhs.local_space);
    swap(lhs.spawn, rhs.spawn);
    swap(lhs.duration, rhs.duration);
    swap(lhs.done, rhs.done);
    swap(lhs.start_time, rhs.start_time);
    swap(lhs.last_time, rhs.last_time);
    swap(lhs.first_update, rhs.first_update);
    swap(lhs.modules, rhs.modules);
    swap(lhs.particles, rhs.particles);
}

void Emitter::accept([[maybe_unused]] EmitterVisitor &visitor) noexcept {

}
