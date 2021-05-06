#pragma once

#include <limitless/fx/renderers/emitter_renderer.hpp>
#include <limitless/fx/emitters/unique_emitter.hpp>

#include <map>
#include <memory>

namespace Limitless {
    class AbstractInstance;
    class UniformSetter;
    class Context;
    class Assets;

    using Instances = std::vector<std::reference_wrapper<AbstractInstance>>;

    namespace ms {
        enum class Blending;
    }
}

namespace Limitless::fx {
    class AbstractEmitterRenderer;

    class EffectRenderer final {
    private:
        std::map<UniqueEmitter, std::unique_ptr<AbstractEmitterRenderer>> renderers;

        void updateRenderers(const Instances& instances) noexcept;
        static void visitEmitters(const Instances& instances, EmitterVisitor& visitor) noexcept;
    public:
        explicit EffectRenderer(Context& context) noexcept;
        ~EffectRenderer() = default;

        void update(const Instances& instances);
        void draw(Context& ctx, const Assets& assets, ShaderPass shader, ms::Blending blending, const UniformSetter& setter);
    };
}