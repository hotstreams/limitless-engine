#pragma once

#include <emitter.hpp>

namespace GraphicsEngine {
    struct UniqueSpriteEmitter;
    class Material;

    class SpriteEmitter : public Emitter {
    protected:
        std::shared_ptr<Material> material;
        //SpriteEmitter() noexcept;
        friend void swap(SpriteEmitter& lhs, SpriteEmitter& rhs) noexcept;
    public:
        SpriteEmitter() noexcept;
        ~SpriteEmitter() override = default;

        SpriteEmitter(const SpriteEmitter&) noexcept = default;
        SpriteEmitter& operator=(const SpriteEmitter&) noexcept = default;

        SpriteEmitter(SpriteEmitter&&) noexcept;
        SpriteEmitter& operator=(SpriteEmitter&&) noexcept;

        [[nodiscard]] SpriteEmitter* clone() const noexcept override;

        [[nodiscard]] auto& getMaterial() noexcept { return *material; }
        [[nodiscard]] const auto& getMaterial() const noexcept { return *material; }

        UniqueSpriteEmitter getEmitterType() const noexcept;

        void accept(EmitterVisiter& visiter) noexcept override;
    };

    void swap(SpriteEmitter& lhs, SpriteEmitter& rhs) noexcept;
}
