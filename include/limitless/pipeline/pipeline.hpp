#pragma once

#include <limitless/pipeline/render_pass.hpp>

#include <vector>
#include <memory>

namespace Limitless {
    class RenderSettings;
    class ContextEventObserver;

    class Pipeline {
    protected:
        std::vector<std::unique_ptr<RenderPass>> passes;
    public:
        Pipeline() = default;
        virtual ~Pipeline() = default;

        template<typename Pass, typename... Args>
        Pass& add(Args&&... args) {
            auto* prev = passes.empty() ? nullptr : passes.back().get();
            auto* pass = new Pass(prev, std::forward<Args>(args)...);
            passes.emplace_back(pass);
            return *pass;
        }

        virtual void update(ContextEventObserver& ctx, Scene& scene, const RenderSettings& settings);

        void clear();
        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);
    };
}