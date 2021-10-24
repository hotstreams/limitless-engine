#pragma once

#include <limitless/pipeline/render_pass.hpp>

#include <vector>
#include <memory>
#include <stdexcept>

namespace Limitless {
    class RenderSettings;
    class ContextEventObserver;

    class pipeline_pass_not_found final : public std::logic_error {
    public:
        explicit pipeline_pass_not_found(const char* msg) : logic_error(msg) {}
        ~pipeline_pass_not_found() = default;
    };

    class Pipeline {
    protected:
        std::vector<std::unique_ptr<RenderPass>> passes;
    public:
        Pipeline() = default;
        virtual ~Pipeline() = default;

        template<typename Pass, typename... Args>
        Pass& add(Args&&... args) {
            auto* pass = new Pass(*this, std::forward<Args>(args)...);
            passes.emplace_back(pass);
            return *pass;
        }

        template<typename Pass>
        auto& get() {
            for (const auto& pass : passes) {
                if (auto *p = dynamic_cast<Pass*>(pass.get()); p) {
                    return *p;
                }
            }

            throw pipeline_pass_not_found(typeid(Pass).name());
        }

        virtual void update(ContextEventObserver& ctx, const RenderSettings& settings);
        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);

        void clear();
    };
}