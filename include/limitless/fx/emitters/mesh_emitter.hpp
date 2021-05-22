#pragma once

#include <limitless/fx/emitters/emitter.hpp>

namespace Limitless {
    class AbstractMesh;
    class EmitterSerializer;

    namespace ms {
        class Material;
    }
}

namespace Limitless::fx {
    struct UniqueMeshEmitter;

    class MeshEmitter : public Emitter<MeshParticle> {
    private:
        std::shared_ptr<AbstractMesh> mesh;
        std::shared_ptr<ms::Material> material;

        MeshEmitter() noexcept;

        friend class EffectBuilder;
        friend class Limitless::EmitterSerializer;
    public:
        ~MeshEmitter() override = default;

        MeshEmitter(const MeshEmitter&);
        MeshEmitter(MeshEmitter&&) noexcept = default;

        [[nodiscard]] UniqueEmitterRenderer getUniqueRendererType() const noexcept override;

        [[nodiscard]] const auto& getParticles() const noexcept { return particles; }

        [[nodiscard]] auto& getMaterial() noexcept { return *material; }
        [[nodiscard]] const auto& getMesh() const noexcept { return mesh; }
        [[nodiscard]] const auto& getMaterial() const noexcept { return* material; }

        [[nodiscard]] MeshEmitter* clone() const override;

        void update(Context& context, const Camera& camera) override;
        void accept(EmitterVisitor& visitor) noexcept override;
    };
}