#pragma once

#include <limitless/pipeline/render_pass.hpp>

#include <vector>
#include <memory>

namespace Limitless {
    class Pipeline {
    private:
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

        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);
    };
}