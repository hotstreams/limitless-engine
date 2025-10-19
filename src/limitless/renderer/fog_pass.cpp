#include <limitless/renderer/fog_pass.hpp>

#include <limitless/assets.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/deferred_lighting_pass.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/renderer/translucent_pass.hpp>
#include <limitless/core/cpu_profiler.hpp>

using namespace Limitless;

FogPass::FogPass(Renderer& renderer)
	: RendererPass {renderer}
	, framebuffer {Framebuffer::asRGB8LinearClampToEdge(renderer.getResolution())} {
}

std::shared_ptr<Texture> FogPass::getResult() {
	return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void FogPass::render(
		[[maybe_unused]] InstanceRenderer& instance_renderer,
		[[maybe_unused]] Scene &scene,
		Context &ctx,
		const Assets &assets,
		[[maybe_unused]] const Camera &camera,
		[[maybe_unused]] UniformSetter &setter) {

	CpuProfileScope scope(global_profiler, "FogPass::render");

	ctx.disable(Capabilities::DepthTest);
	ctx.disable(Capabilities::Blending);

	ctx.setViewPort(getResult()->getSize());
	framebuffer.clear();

	auto& shader = assets.shaders.get("fog");

	shader
		.setUniform("lightened", renderer.getPass<TranslucentPass>().getResult())
		.setUniform("depth_texture", renderer.getPass<DeferredFramebufferPass>().getDepth())
		.setUniform("fog_color", fog_color)
		.setUniform("fog_start", fog_start)
		.setUniform("fog_end", fog_end)
		.setUniform("fog_height_start", fog_height_start)
		.setUniform("fog_height_end", fog_height_end)
		.setUniform("fog_density", fog_density)
		.setUniform("fog_enabled", fog_enabled ? 1 : 0)
		.use();

	assets.meshes.at("quad")->draw();
}

void FogPass::onFramebufferChange(glm::uvec2 size) {
	framebuffer.onFramebufferChange(size);
}


