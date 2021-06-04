#include <limitless/fx/emitters/emitter.hpp>

using namespace Limitless::fx;

template<typename Particle>
bool ModuleCompare<Particle>::operator() (const std::unique_ptr<Module<Particle>>& lhs, const std::unique_ptr<Module<Particle>>& rhs) const {
    return lhs->getType() < rhs->getType();
}

template<typename P>
Emitter<P>::Emitter(Type _type)
    : AbstractEmitter(_type) {
}

template<typename P>
void Emitter<P>::emit(uint32_t count) noexcept {
    for (uint32_t i = 0; i < count; ++i) {
        P particle {};

        particle.getPosition() = local_position + position;
        particle.getRotation() = glm::eulerAngles(rotation * local_rotation);

        for (auto& module : modules) {
            module->initialize(*this, particle, particles.size());
        }

        particles.emplace_back(particle);
    }
}

template<typename P>
const glm::vec3& Emitter<P>::getPosition() const noexcept {
    return position;
}

template<typename P>
const glm::quat& Emitter<P>::getRotation() const noexcept {
    return rotation;
}

template<typename P>
void Emitter<P>::kill() noexcept {
    done = true;
}

template<typename P>
void Emitter<P>::ressurect() noexcept {
    done = false;
}

template<typename P>
bool& Emitter<P>::getLocalSpace() noexcept {
    return local_space;
}

template<typename P>
EmitterSpawn& Emitter<P>::getSpawn() noexcept {
    return spawn;
}

template<typename P>
glm::vec3& Emitter<P>::getLocalPosition() noexcept {
    return local_position;
}

template<typename P>
glm::quat& Emitter<P>::getLocalRotation() noexcept {
    return local_rotation;
}

template<typename P>
std::chrono::duration<float>& Emitter<P>::getDuration() noexcept {
    return duration;
}

template<typename P>
bool Emitter<P>::isDone() const noexcept {
    return false;
}

template<typename P>
bool Emitter<P>::getLocalSpace() const noexcept {
    return local_space;
}

template<typename P>
const glm::vec3& Emitter<P>::getLocalPosition() const noexcept {
    return local_position;
}

template<typename P>
const glm::quat& Emitter<P>::getLocalRotation() const noexcept {
    return local_rotation;
}

template<typename P>
const EmitterSpawn& Emitter<P>::getSpawn() const noexcept {
    return spawn;
}

template<typename P>
const std::chrono::duration<float>& Emitter<P>::getDuration() const noexcept {
    return duration;
}

template<typename P>
void Emitter<P>::setPosition(const glm::vec3& new_position) noexcept {
    if (local_space) {
        const auto final_position = new_position + local_position;
        const auto diff = final_position - (position + local_position);

        for (auto& particle : particles) {
            particle.getPosition() += diff;
        }
    }

    position = new_position;
}

template<typename P>
void Emitter<P>::setRotation(const glm::quat& new_rotation) noexcept {
    if (local_space) {
        const auto final_rotation = new_rotation * local_rotation;
        const auto diff = final_rotation * glm::inverse(rotation * local_rotation);

        for (auto& particle : particles) {
            particle.getRotation() += glm::eulerAngles(diff);

            particle.getVelocity() = diff * particle.getVelocity();
            particle.getAcceleration() = diff * particle.getAcceleration();
        }
    }

    rotation = new_rotation;
}

template<typename P>
void Emitter<P>::spawnParticles() noexcept {
    if (spawn.spawn_rate <= 0.0f) {
        return;
    }

    const auto current_time = std::chrono::steady_clock::now();
    if (spawn.last_spawn == std::chrono::time_point<std::chrono::steady_clock>()) {
        spawn.last_spawn = current_time;
    }
    const auto delta = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - spawn.last_spawn).count();

    switch (spawn.mode) {
        case EmitterSpawn::Mode::Spray: {
            if (delta >= (1.0f / spawn.spawn_rate)) {
                const int remaining = spawn.max_count - particles.size();
                if (remaining > 0) {
                    emit(glm::clamp(static_cast<int>(delta * spawn.spawn_rate), 1, remaining));
                }
                spawn.last_spawn = current_time;
            }
            break;
        }
        case EmitterSpawn::Mode::Burst:
            if (spawn.burst->loops != spawn.burst->loops_done) {
                if ((current_time - spawn.last_spawn).count() >= spawn.spawn_rate) {
                    auto emit_count = spawn.burst->burst_count->get();
                    emit_count = (particles.size() + emit_count > spawn.max_count) ? spawn.max_count - particles.size() : emit_count;
                    emit(emit_count);

                    if (spawn.burst->loops != -1) {
                        ++spawn.burst->loops_done;
                    }

                    spawn.last_spawn = current_time;
                }
            }
            break;
    }
}

template<typename P>
void Emitter<P>::killParticles() noexcept {
    std::vector<size_t> indices;
    for (size_t i = 0; i < particles.size(); ++i) {
        if (particles[i].getLifetime() <= 0.0f) {
            indices.emplace_back(i);
        }
    }

    for (auto it = particles.begin(); it != particles.end();) {
        if (it->getLifetime() <= 0.0f) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }

    for (auto& module : modules) {
        module->deinitialize(indices);
    }
}

template<typename P>
void Emitter<P>::update([[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {
    using namespace std::chrono;

    const auto current_time = steady_clock::now();
    const auto delta_time = duration_cast<std::chrono::duration<float>>(current_time - last_time);
    last_time = current_time;

	if (start_time == time_point<steady_clock>()) {
		start_time = current_time;
	}

    killParticles();

    {
        for (auto& module : modules) {
            module->update(*this, particles, delta_time.count(), ctx, camera);
        }

        for (auto& particle : particles) {
            particle.getPosition() += particle.getVelocity() * delta_time.count();
            particle.getVelocity() += particle.getAcceleration() * delta_time.count();
        }
    }

    if (!done) {
        spawnParticles();
    }

    if (duration.count() != 0.0f) {
        if (current_time - start_time >= duration) {
            done = true;
        }
    }
}

template<typename P>
Emitter<P>* Emitter<P>::clone() const {
    return new Emitter(*this);
}

template<typename P>
Emitter<P>::Emitter(const Emitter& emitter)
    : AbstractEmitter(emitter)
    , local_position {emitter.local_position}
    , local_rotation {emitter.local_rotation}
    , position {emitter.position}
    , rotation {emitter.rotation}
    , local_space {emitter.local_space}
    , spawn {emitter.spawn}
    , duration {emitter.duration}
    , unique_shader {emitter.unique_shader} {
    // deep modules copy
    for (const auto& module : emitter.modules) {
        modules.emplace(module->clone());
    }
}

template<typename P>
void Emitter<P>::accept([[maybe_unused]] EmitterVisitor &visitor) noexcept {
}

namespace Limitless::fx {
    template class Emitter<SpriteParticle>;
    template class Emitter<MeshParticle>;
    template class Emitter<BeamParticle>;

    template bool ModuleCompare<SpriteParticle>::operator()(const std::unique_ptr<Module<SpriteParticle>>&, const std::unique_ptr<Module<SpriteParticle>>&) const;
    template bool ModuleCompare<MeshParticle>::operator()(const std::unique_ptr<Module<MeshParticle>>&, const std::unique_ptr<Module<MeshParticle>>&) const;
    template bool ModuleCompare<BeamParticle>::operator()(const std::unique_ptr<Module<BeamParticle>>&, const std::unique_ptr<Module<BeamParticle>>&) const;
}
