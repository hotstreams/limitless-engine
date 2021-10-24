#pragma once

#include <limitless/pipeline/pipeline.hpp>

namespace Limitless {
    class ContextEventObserver;
    class RenderSettings;

    class Deferred final : public Pipeline {
    private:
        void create(ContextEventObserver& ctx, const RenderSettings& settings);
    public:
        Deferred(ContextEventObserver& ctx, const RenderSettings& settings);
        ~Deferred() override = default;

        void update(ContextEventObserver& ctx, const RenderSettings& settings) override;
    };
}