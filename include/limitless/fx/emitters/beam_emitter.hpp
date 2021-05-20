#pragma once

#include <limitless/fx/emitters/emitter.hpp>

namespace Limitless {
    class EmitterSerializer;
}

namespace Limitless::ms {
    class Material;
}

namespace Limitless::fx {
    class BeamEmitter : public Emitter<BeamParticle> {
    private:
        std::shared_ptr<ms::Material> material;

        BeamEmitter() noexcept;

        friend class EffectBuilder;
        friend class Limitless::EmitterSerializer;
    public:
        ~BeamEmitter() override = default;

        BeamEmitter(const BeamEmitter&);
        BeamEmitter(BeamEmitter&&) noexcept = default;

        [[nodiscard]] const std::vector<BeamParticleMapping>& getParticles() const noexcept;

        [[nodiscard]] ms::Material& getMaterial() noexcept { return *material; }
        [[nodiscard]] const ms::Material& getMaterial() const noexcept { return *material; }

        [[nodiscard]] BeamEmitter* clone() const noexcept override;

        void accept(EmitterVisitor& visitor) noexcept override;
    };
}