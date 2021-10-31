#include <limitless/pipeline/outline_pass.hpp>
//#include <limitless/instances/abstract_instance.hpp>
//#include <limitless/pipeline/shader_pass_types.hpp>
//#include <limitless/ms/blending.hpp>
//#include <limitless/instances/model_instance.hpp>
//#include <limitless/assets.hpp>
//
//using namespace Limitless;
//
//OutlinePass::OutlinePass(Pipeline& pipeline)
//	: RenderPass(pipeline) {
//
//}
//
//void OutlinePass::draw(Instances& instances, Limitless::Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) {
//	ctx.disable(Capabilities::DepthTest);
//	ctx.enable(Capabilities::StencilTest);
//
//	ctx.setStencilFunc(StencilFunc::Nequal, 1, 0xFF);
//	ctx.setStencilMask(0x00);
//
//	for (const auto& instance : instances) {
//		if (instance.get().isOutlined()) {
//			if (instance.get().getShaderType() == ModelShader::Model ||
//				instance.get().getShaderType() == ModelShader::Skeletal) {
//				auto& model_instance = static_cast<ModelInstance&>(instance.get());
//
//				for (auto&[_, mesh] : model_instance.getMeshes()) {
//					mesh.getMaterial().changeMaterial(assets.materials.at("outline"));
//				}
//
//				const auto scale = instance.get().getScale();
//				instance.get().setScale(scale * 1.05f);
//				instance.get().updateModelMatrix();
//				instance.get().updateFinalMatrix();
//				instance.get().draw(ctx, assets, ShaderPass::Forward, ms::Blending::Opaque);
//				instance.get().setScale(scale);
//
//				for (auto&[_, mesh] : model_instance.getMeshes()) {
//					mesh.getMaterial().reset();
//				}
//			}
//		}
//	}
//}
