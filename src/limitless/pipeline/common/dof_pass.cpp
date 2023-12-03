#include <limitless/pipeline/common/dof_pass.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include "limitless/core/shader/shader_program.hpp"

using namespace Limitless;

DoFPass::DoFPass(Pipeline& pipeline, ContextEventObserver& ctx, RenderTarget& _target)
	: PipelinePass {pipeline}
	, blur {ctx.getSize()}
	, target {_target} {
}

DoFPass::DoFPass(Pipeline& pipeline, ContextEventObserver& ctx)
	: PipelinePass {pipeline}
	, blur {ctx.getSize()}
	, target {framebuffer} {
	initialize(ctx);
}

void DoFPass::initialize(Context& ctx) {
	auto result = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB8)
            .size(ctx.getSize())
            .format(Texture::Format::RGB)
            .data_type(Texture::DataType::UnsignedByte)
            .min_filter(Texture::Filter::Linear)
            .mag_filter(Texture::Filter::Linear)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
			.build();

	framebuffer.bind();
	framebuffer << TextureAttachment{FramebufferAttachment::Color0, result};
	framebuffer.checkStatus();
	framebuffer.unbind();
}

void DoFPass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
	{
		blur.process(ctx, assets, getPreviousResult());
	}

	{
		target.clear();

		auto& shader = assets.shaders.get("dof");
		auto& gbuffer = pipeline.get<DeferredFramebufferPass>();

		shader .setUniform( "depth_texture", gbuffer.getDepth())
               .setUniform( "focus_texture", getPreviousResult())
               .setUniform( "unfocus_texture", blur.getResult())
               .setUniform( "uv_focus", focus)
               .setUniform( "distance", distance);

		shader.use();

		assets.meshes.at("quad")->draw();
	}
}
