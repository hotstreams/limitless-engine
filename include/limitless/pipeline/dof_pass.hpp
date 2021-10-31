#pragma once

#include <limitless/core/framebuffer.hpp>
#include <limitless/pipeline/render_pass.hpp>
#include <limitless/pipeline/postprocessing.hpp>

namespace Limitless {
	class DoFPass final : public RenderPass {
	private:
		Framebuffer framebuffer;
		Blur blur;
		RenderTarget& target;
		glm::vec2 focus {0.7f, 0.5f};
		glm::vec2 distance {0.3f, 0.55f};
		void initialize(Context& ctx);
	public:
		DoFPass(Pipeline& pipeline, ContextEventObserver& ctx, RenderTarget& target);
		DoFPass(Pipeline& pipeline, ContextEventObserver& ctx);
		~DoFPass() override = default;

		auto& getFocus() noexcept { return focus; }
		const auto& getFocus() const noexcept { return focus; }

		std::shared_ptr<Texture> getResult() override { return framebuffer.get(FramebufferAttachment::Color0).texture; }

		void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
	};
}
