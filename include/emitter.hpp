#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>

#include <emitter_module.hpp>
#include <emitter_spawn.hpp>
#include <particle.hpp>

namespace GraphicsEngine {
    class EmitterVisiter;

    enum class EmitterType { Sprite, Mesh, /*Beam, GPU*/ };

    class Emitter {
    protected:
        EmitterType type;
        glm::vec3 local_position, position;
        glm::vec3 local_rotation, rotation;
        bool local_space;

        EmitterSpawn spawn;
        std::chrono::duration<float> duration {0.0f};
        bool done {false};

        std::chrono::time_point<std::chrono::steady_clock> start_time;
        std::chrono::time_point<std::chrono::steady_clock> last_time;
        bool first_update {true};

        std::unordered_map<EmitterModuleType, std::unique_ptr<EmitterModule>> modules;
        std::vector<Particle> particles;

        void emit(uint32_t count) noexcept;
        void spawnParticles() noexcept;
        void killParticles() noexcept;

        friend void swap(Emitter& lhs, Emitter& rhs) noexcept;
        explicit Emitter(EmitterType type) noexcept;
        Emitter() noexcept;
    public:
        virtual ~Emitter() = default;

        Emitter(const Emitter&) noexcept;
        Emitter& operator=(const Emitter&) noexcept;

        Emitter(Emitter&&) noexcept;
        Emitter& operator=(Emitter&&) noexcept;

        [[nodiscard]] virtual Emitter* clone() const noexcept;

        [[nodiscard]] const auto& getParticles() const noexcept { return particles; }
        [[nodiscard]] const auto& getPosition() const noexcept { return position; }
        [[nodiscard]] const auto& getRotation() const noexcept { return rotation; }
        [[nodiscard]] const auto& getModules() const noexcept { return modules; }
        [[nodiscard]] auto getType() const noexcept { return type; }

        auto& getLocalPosition() noexcept { return local_position; }
        auto& getLocalRotation() noexcept { return local_rotation; }
        auto& getLocalSpace() noexcept { return local_space; }
        auto& getDuration() noexcept { return duration; }

        auto& getSpawn() noexcept { return spawn; }
        const auto& getSpawn() const noexcept { return spawn; }

        void setPosition(const glm::vec3& position) noexcept;
        void setRotation(const glm::vec3& rotation) noexcept;

        [[nodiscard]] auto isDone() const noexcept { return done; }
        void kill() noexcept { done = true; }
        void ressurect() noexcept { done = false; }

        virtual void update();

        virtual void accept(EmitterVisiter& visiter) noexcept;
    };

    void swap(Emitter& lhs, Emitter& rhs) noexcept;
}
