#include <emitter.hpp>

using namespace GraphicsEngine;

void GraphicsEngine::swap(EmitterSpawn& lhs, EmitterSpawn& rhs) noexcept {
    using std::swap;

    swap(lhs.mode, rhs.mode);
    swap(lhs.last_spawn, rhs.last_spawn);
    swap(lhs.max_count, rhs.max_count);
    swap(lhs.spawn_rate, rhs.spawn_rate);
    swap(lhs.burst_count, rhs.burst_count);
    swap(lhs.loops, rhs.loops);
    swap(lhs.loops_done, rhs.loops_done);
}

EmitterSpawn::EmitterSpawn(const EmitterSpawn& emitter) noexcept
    : mode{emitter.mode}, last_spawn{emitter.last_spawn},
      max_count{emitter.max_count}, spawn_rate{emitter.spawn_rate},
      burst_count{emitter.burst_count->clone()}, loops{emitter.loops},
      loops_done{emitter.loops_done} {

}

Emitter::Emitter(EmitterType type) noexcept
    : local_position{0.0f}, position{0.0f},
      local_rotation{0.0f}, rotation{0.0f}, local_space{false}, type{type} {

}

Emitter::Emitter() noexcept
    : local_position{0.0f}, position{0.0f},
      local_rotation{0.0f}, rotation{0.0f}, local_space{false}, type{EmitterType::Sprite} {

}

void Emitter::emit(uint32_t count) noexcept {
    for (uint32_t i = 0; i < count; ++i) {
        Particle particle;

        particle.position = local_position + position;
        particle.angle = local_rotation + rotation;

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

void Emitter::setRotation(const glm::vec3& new_rotation) noexcept {
    if (local_space) {
        auto diff = new_rotation + local_rotation - rotation;

        for (auto& particle : particles) {
            particle.angle += diff;
        }
    }

    rotation = local_rotation + new_rotation;
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
    if (done) {
        return;
    }

    if (first_update) {
        last_time = std::chrono::steady_clock::now();
        start_time = last_time;
        first_update = false;
    }

    auto current_time = std::chrono::steady_clock::now();
    auto delta_time = current_time - last_time;
    last_time = current_time;

    spawnParticles();
    killParticles();

    for (auto& [type, module] : modules) {
        module->update(*this, particles, delta_time.count());
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
    : local_position{emitter.local_position}, position{emitter.position},
      local_rotation{emitter.local_rotation}, rotation{emitter.rotation},
      local_space{emitter.local_space}, spawn{emitter.spawn}, duration{emitter.duration} {
    modules.reserve(emitter.modules.size());
    for (const auto& [type, module] : emitter.modules) {
        modules.emplace(type, module->clone());
    }
}

Emitter& Emitter::operator=(const Emitter& emitter) noexcept {
    auto copy = Emitter(emitter);
    swap(*this, copy);
    return *this;
}

void GraphicsEngine::swap(Emitter& lhs, Emitter& rhs) noexcept {
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

Emitter::Emitter(Emitter&& emitter) noexcept
    : Emitter() {
    swap(*this, emitter);
}

Emitter &Emitter::operator=(Emitter&& emitter) noexcept {
    swap(*this, emitter);
    return *this;
}
