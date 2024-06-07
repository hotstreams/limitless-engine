#pragma once

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class Context;
    class RendererSettings;

    class Deferred final : public Pipeline {
    private:
        void build(Context& ctx, const RendererSettings& settings);
    public:
        Deferred(Context& ctx, glm::uvec2 size, const RendererSettings& settings, RenderTarget& target = default_framebuffer);

	    void update(Context& ctx, const RendererSettings& settings) override;
    };
}
