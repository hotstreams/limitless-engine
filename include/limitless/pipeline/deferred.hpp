#pragma once

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class ContextEventObserver;
    class RenderSettings;

    class Deferred final : public Pipeline {
    private:
        void build(ContextEventObserver& ctx, const RenderSettings& settings);
    public:
        Deferred(ContextEventObserver& ctx, glm::uvec2 size, const RenderSettings& settings, RenderTarget& target = default_framebuffer);

	    void update(ContextEventObserver& ctx, const RenderSettings& settings) override;
    };
}
