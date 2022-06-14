#pragma once

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class ContextEventObserver;
    class RenderSettings;

    using RTRef = std::reference_wrapper<RenderTarget>;

    class Deferred final : public Pipeline {
    private:
        RTRef target;
        void build(ContextEventObserver& ctx, const RenderSettings& settings);
    public:
        Deferred(ContextEventObserver& ctx, glm::uvec2 size, const RenderSettings& settings, RenderTarget& target = default_framebuffer);

        void setTarget(RTRef target) noexcept;
        auto& getTarget() noexcept { return target; }

	    void update(ContextEventObserver& ctx, const RenderSettings& settings) override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}
