#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>

#include <emitter_module.hpp>

namespace GraphicsEngine {
    struct EmitterSpawn {
        enum class Mode { Spray, Burst } mode {Mode::Spray};

        std::chrono::time_point<std::chrono::steady_clock> last_spawn;

        uint32_t max_count {100};
        float spawn_rate {1.0f};

        std::unique_ptr<Distribution<uint32_t>> burst_count;
        int loops {-1};
        uint16_t loops_done {0};

        friend void swap(EmitterSpawn& lhs, EmitterSpawn& rhs) noexcept;

        EmitterSpawn() = default;

        EmitterSpawn(const EmitterSpawn&) noexcept;
        EmitterSpawn(EmitterSpawn&&) noexcept = default;
    };
    void swap(EmitterSpawn& lhs, EmitterSpawn& rhs) noexcept;

    struct Particle {
        glm::vec3 position {0.0f};
        glm::vec4 color {1.0f};
        glm::vec3 angle {0.0f};
        glm::vec3 velocity {0.0f};
        glm::vec3 acceleration {0.0f};
        float lifetime {1.0f};
        float size {256.0f};
        // xy - scaling factor; zw - frame uv
        glm::vec4 subUV {1.0f};
        // custom per particle prop materials
        glm::vec4 properties {1.0f};
    };

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
        [[nodiscard]] const auto& getType() const noexcept { return type; }

        auto& getLocalPosition() noexcept { return local_position; }
        auto& getLocalRotation() noexcept { return local_rotation; }
        auto& getLocalSpace() noexcept { return local_space; }
        auto& getDuration() noexcept { return duration; }
        auto& getSpawn() noexcept { return spawn; }

        void setPosition(const glm::vec3& position) noexcept;
        void setRotation(const glm::vec3& rotation) noexcept;

        [[nodiscard]] auto isDone() const noexcept { return done; }
        void kill() noexcept { done = true; }
        void ressurect() noexcept { done = false; }

        virtual void update();
    };

    void swap(Emitter& lhs, Emitter& rhs) noexcept;
}
