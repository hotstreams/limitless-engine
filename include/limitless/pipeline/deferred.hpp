#pragma once

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class ContextEventObserver;
    class RenderSettings;

    class Deferred final : public Pipeline {
    private:
    	RenderTarget& target;
        bool dep {};
        void buildDependent(ContextEventObserver& ctx, const RenderSettings& settings);
        void buildIndependent(ContextEventObserver& ctx, glm::uvec2 frame_size, const RenderSettings& settings);
    public:
        Deferred(ContextEventObserver& ctx, glm::uvec2 frame_size, const RenderSettings& settings, RenderTarget& target = default_framebuffer);
        Deferred(ContextEventObserver& ctx, const RenderSettings& settings, RenderTarget& target = default_framebuffer);

        auto& getTarget() noexcept { return target; }

	    void update(ContextEventObserver& ctx, glm::uvec2 frame_size, const RenderSettings& settings) override;
    };
}