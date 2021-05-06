#pragma once

#include <limitless/pipeline/pipeline.hpp>

namespace Limitless {
    class ContextEventObserver;

    class Forward final : public Pipeline {
    private:
    public:
        explicit Forward(ContextEventObserver& ctx, Scene& scene);
        ~Forward() override = default;
    };
}