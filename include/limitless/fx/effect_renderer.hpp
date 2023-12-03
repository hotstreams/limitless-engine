#pragma once

#include <limitless/fx/renderers/emitter_renderer.hpp>
#include <limitless/fx/emitters/unique_emitter.hpp>

#include <map>
#include <memory>

namespace Limitless {
    class Instance;
    class UniformSetter;
    class Context;
    class Assets;

    using Instances = std::vector<std::reference_wrapper<Instance>>;

    namespace ms {
        enum class Blending;
    }
}

namespace Limitless::fx {
    class AbstractEmitterRenderer;

    class EffectRenderer final {
    private:
        std::map<UniqueEmitterRenderer, std::unique_ptr<AbstractEmitterRenderer>> renderers;

        void updateRenderers(const Instances& instances) noexcept;
        static void visitEmitters(const Instances& instances, EmitterVisitor& visitor) noexcept;
    public:
        explicit EffectRenderer(Context& context) noexcept;
        ~EffectRenderer() = default;

        void update(const Instances& instances);
        void draw(Context& ctx, const Assets& assets, ShaderType shader, ms::Blending blending, const UniformSetter& setter);
    };
}