#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <stdexcept>

namespace Limitless {
    class RenderSettings;
    class ContextEventObserver;

    class pipeline_pass_not_found final : public std::logic_error {
    public:
        using std::logic_error::logic_error;
    };

    class Pipeline {
    protected:
        std::vector<std::unique_ptr<RenderPass>> passes;
        glm::uvec2 size;
    public:
        explicit Pipeline(glm::uvec2 size) : size {size} {}
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

        auto& getPrevious(RenderPass* curr) {
	        for (uint32_t i = 1; i < passes.size(); ++i) {
		        if (passes[i].get() == curr) {
		        	return passes[i - 1];
		        }
	        }

	        throw pipeline_pass_not_found {"There is no previous pass!"};
        }

        virtual void update(ContextEventObserver& ctx, const RenderSettings& settings);

        virtual void onFramebufferChange(glm::uvec2 size);

        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);
        void clear();
    };
}