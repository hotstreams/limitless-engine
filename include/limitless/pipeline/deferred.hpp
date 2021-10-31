#pragma once

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class ContextEventObserver;
    class RenderSettings;

    class Deferred final : public Pipeline {
    private:
    	RenderTarget& target;
        void create(ContextEventObserver& ctx, const RenderSettings& settings);
    public:
        Deferred(ContextEventObserver& ctx, const RenderSettings& settings);
        Deferred(ContextEventObserver& ctx, const RenderSettings& settings, RenderTarget& target);
        ~Deferred() override = default;

        auto& getTarget() noexcept { return target; }

	    void update(ContextEventObserver& ctx, const RenderSettings& settings) override;
    };
}