#pragma once

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/fx/emitters/emitter_spawn.hpp>
#include <limitless/fx/particle.hpp>

#include <glm/gtx/quaternion.hpp>

#include <memory>
#include <set>

#include <limitless/fx/emitters/unique_emitter.hpp>

namespace Limitless::fx {
    template<typename Particle> class Module;

    template<typename Particle>
    struct ModuleCompare {
        bool operator()(const std::unique_ptr<Module<Particle>>& lhs, const std::unique_ptr<Module<Particle>>& rhs) const;
    };

    template<typename Particle = SpriteParticle>
    class Emitter : public AbstractEmitter {
    protected:
        // emitter modules determine particles appearance and behavior
        std::set<std::unique_ptr<Module<Particle>>, ModuleCompare<Particle>> modules;
        std::vector<Particle> particles;

        // local position of emitter
        glm::vec3 local_position {0.0f};
        glm::quat local_rotation {1.0f, 0.0f, 0.0f, 0.0f};

        // current position of emitter
        glm::vec3 position {0.0f};
        glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};

        // particles position is relative to emitter
        bool local_space {false};

        // spawn properties
        EmitterSpawn spawn;

        // emitter duration in seconds; 0 for infinity
        std::chrono::duration<float> duration {0.0f};

        std::chrono::time_point<std::chrono::steady_clock> start_time {};
        std::chrono::time_point<std::chrono::steady_clock> last_time {};

        bool done {false};

        UniqueEmitterShader unique_shader;
        UniqueEmitterRenderer unique_renderer;

        void emit(uint32_t count) noexcept;
        void spawnParticles() noexcept;
        void killParticles() noexcept;

        explicit Emitter(Type type);
        ~Emitter() override = default;

        Emitter(const Emitter&);
        Emitter(Emitter&&) noexcept = default;

        friend class EffectBuilder;
    public:
        [[nodiscard]] const glm::vec3& getPosition() const noexcept override;
        [[nodiscard]] const glm::quat& getRotation() const noexcept override;

        void setPosition(const glm::vec3& position) noexcept override;
        void setRotation(const glm::quat& rotation) noexcept override;

        void kill() noexcept override;
        void ressurect() noexcept override;

        [[nodiscard]] const UniqueEmitterShader& getUniqueShaderType() const noexcept override { return unique_shader; }
        [[nodiscard]] const UniqueEmitterRenderer& getUniqueRendererType() const noexcept override { return unique_renderer; }

        [[nodiscard]] Emitter* clone() const override;
        void update(Context& ctx, const Camera& camera) override;
        void accept(EmitterVisitor& visitor) noexcept override;

        bool& getLocalSpace() noexcept override;
        EmitterSpawn& getSpawn() noexcept override;
        glm::vec3& getLocalPosition() noexcept override;
        glm::quat& getLocalRotation() noexcept override;
        std::chrono::duration<float>& getDuration() noexcept override;

        [[nodiscard]] bool isDone() const noexcept override;
        [[nodiscard]] bool getLocalSpace() const noexcept override;
        [[nodiscard]] const glm::vec3& getLocalPosition() const noexcept override;
        [[nodiscard]] const glm::quat& getLocalRotation() const noexcept override;
        [[nodiscard]] const EmitterSpawn& getSpawn() const noexcept override;
        [[nodiscard]] const std::chrono::duration<float>& getDuration() const noexcept override;
    };
}
