#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
	class OutlinePass : public PipelinePass {
    private:
        Framebuffer framebuffer;
	public:
        glm::vec3 outline_color = glm::vec3(1.0, 0.0, 0.0);
        int32_t width = 2;

		explicit OutlinePass(Pipeline& pipeline, glm::uvec2 size);

        std::shared_ptr<Texture> getResult() override;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}