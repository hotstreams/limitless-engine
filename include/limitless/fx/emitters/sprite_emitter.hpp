#pragma once

#include <limitless/fx/emitters/emitter.hpp>

namespace Limitless::ms {
    class Material;
}

namespace Limitless::fx {
    class SpriteEmitter : public Emitter<> {
    protected:
        std::shared_ptr<ms::Material> material;

        SpriteEmitter() noexcept;

        friend class EffectBuilder;
        friend class EmitterSerializer;
    public:
        ~SpriteEmitter() override = default;

        SpriteEmitter(const SpriteEmitter&);
        SpriteEmitter(SpriteEmitter&&) noexcept = default;

        [[nodiscard]] const auto& getParticles() const noexcept { return particles; }

        [[nodiscard]] ms::Material& getMaterial() noexcept { return *material; }
        [[nodiscard]] const ms::Material& getMaterial() const noexcept { return *material; }

        [[nodiscard]] SpriteEmitter* clone() const override;
        void accept(EmitterVisitor& visitor) noexcept override;
    };
}
