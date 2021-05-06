#pragma once

#include <glm/glm.hpp>
#include <chrono>
#include <vector>

namespace Limitless {
    class Context;
    class Camera;
}

namespace Limitless::fx {
    class EmitterVisitor;
    class EmitterSpawn;
    class UniqueEmitter;

    class AbstractEmitter {
    public:
        enum class Type { Sprite, Mesh, Beam };
    protected:
        Type type;

        explicit AbstractEmitter(Type type) noexcept;

        AbstractEmitter(const AbstractEmitter&) = default;
        AbstractEmitter& operator=(const AbstractEmitter&) = default;

        AbstractEmitter(AbstractEmitter&&) = default;
        AbstractEmitter& operator=(AbstractEmitter&&) = default;

        friend class EffectBuilder;
    public:
        virtual ~AbstractEmitter() = default;

        [[nodiscard]] auto getType() const noexcept { return type; }
        [[nodiscard]] virtual const UniqueEmitter& getUniqueType() const noexcept = 0;

        [[nodiscard]] virtual const glm::vec3& getPosition() const noexcept = 0;
        [[nodiscard]] virtual const glm::quat& getRotation() const noexcept = 0;

        virtual void setPosition(const glm::vec3& position) noexcept = 0;
        virtual void setRotation(const glm::quat& rotation) noexcept = 0;

        virtual void kill() noexcept = 0;
        virtual void ressurect() noexcept = 0;

        [[nodiscard]] virtual AbstractEmitter* clone() const = 0;
        virtual void update(Context& ctx, const Camera& camera) = 0;
        virtual void accept(EmitterVisitor& visitor) noexcept = 0;

        virtual bool& getLocalSpace() noexcept = 0;
        virtual EmitterSpawn& getSpawn() noexcept = 0;
        virtual glm::vec3& getLocalPosition() noexcept = 0;
        virtual glm::quat& getLocalRotation() noexcept = 0;
        virtual std::chrono::duration<float>& getDuration() noexcept = 0;

        [[nodiscard]] virtual bool isDone() const noexcept = 0;
        [[nodiscard]] virtual bool getLocalSpace() const noexcept = 0;
        [[nodiscard]] virtual const glm::vec3& getLocalPosition() const noexcept = 0;
        [[nodiscard]] virtual const glm::quat& getLocalRotation() const noexcept = 0;
        [[nodiscard]] virtual const EmitterSpawn& getSpawn() const noexcept  = 0;
        [[nodiscard]] virtual const std::chrono::duration<float>& getDuration() const noexcept = 0;
    };
}
