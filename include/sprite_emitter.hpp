#pragma once

#include <emitter.hpp>

namespace GraphicsEngine {
    class Material;

    struct UniqueSpriteEmitter {
        std::vector<EmitterModuleType> modules;
        std::shared_ptr<Material> material;
    };
    bool operator<(const UniqueSpriteEmitter& lhs, const UniqueSpriteEmitter& rhs) noexcept;
    bool operator==(const UniqueSpriteEmitter& lhs, const UniqueSpriteEmitter& rhs) noexcept;

    class SpriteEmitter : public Emitter {
    protected:
        std::shared_ptr<Material> material;
        SpriteEmitter() noexcept;
        friend void swap(SpriteEmitter& lhs, SpriteEmitter& rhs) noexcept;
    public:
        ~SpriteEmitter() override = default;

        SpriteEmitter(const SpriteEmitter&) noexcept = default;
        SpriteEmitter& operator=(const SpriteEmitter&) noexcept = default;

        SpriteEmitter(SpriteEmitter&&) noexcept;
        SpriteEmitter& operator=(SpriteEmitter&&) noexcept;

        [[nodiscard]] SpriteEmitter* clone() const noexcept override;

        [[nodiscard]] const auto& getMaterial() const noexcept { return material; }

        UniqueSpriteEmitter getEmitterType() const noexcept;
    };

    void swap(SpriteEmitter& lhs, SpriteEmitter& rhs) noexcept;
}
