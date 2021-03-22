#pragma once

#include <limitless/particle_system/emitter.hpp>

namespace LimitlessEngine {
    struct UniqueSpriteEmitter;
    class Material;

    class SpriteEmitter : public Emitter {
    protected:
        std::shared_ptr<Material> material;
        friend void swap(SpriteEmitter& lhs, SpriteEmitter& rhs) noexcept;
        explicit SpriteEmitter(EmitterType type) noexcept;

        friend class EffectBuilder;
        SpriteEmitter() noexcept;
    public:
        ~SpriteEmitter() override = default;

        SpriteEmitter(const SpriteEmitter&) noexcept = default;
        SpriteEmitter& operator=(const SpriteEmitter&) noexcept = default;

        SpriteEmitter(SpriteEmitter&&) noexcept;
        SpriteEmitter& operator=(SpriteEmitter&&) noexcept;

        [[nodiscard]] SpriteEmitter* clone() const noexcept override;

        [[nodiscard]] auto& getMaterial() noexcept { return *material; }
        [[nodiscard]] const auto& getMaterial() const noexcept { return *material; }

        [[nodiscard]] UniqueSpriteEmitter getEmitterType() const noexcept;

        void accept(EmitterVisitor& visitor) noexcept override;
    };

    void swap(SpriteEmitter& lhs, SpriteEmitter& rhs) noexcept;
}
