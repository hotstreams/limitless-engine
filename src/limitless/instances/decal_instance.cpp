#include <limitless/instances/decal_instance.hpp>

#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>

using namespace Limitless;

DecalInstance::DecalInstance(std::shared_ptr<AbstractModel> model, std::shared_ptr<ms::Material> material, const glm::vec3& position)
    : Instance {InstanceType::Decal, position}
    , model {model}
    , material {std::make_shared<ms::Material>(*material)} {
}

void DecalInstance::draw(Context &ctx, const Assets &assets, ShaderType shader_type, ms::Blending blending, const UniformSetter &uniform_set) {
    if (material->getBlending() != blending) {
        return;
    }

    setBlendingMode(material->getBlending());

    ctx.disable(Capabilities::DepthTest);
    ctx.setDepthFunc(DepthFunc::Lequal);
    ctx.setDepthMask(DepthMask::False);

    auto& shader = assets.shaders.get(shader_type, InstanceType::Decal, material->getShaderIndex());

    // updates model/material uniforms
    shader.setUniform("_model_transform", final_matrix)
          .setMaterial(*material);

    // sets custom pass-dependent uniforms
    uniform_set(shader);

    shader.use();

    model->getMeshes()[0]->draw();
}

void DecalInstance::updateBoundingBox() noexcept {

}

std::unique_ptr<Instance> DecalInstance::clone() noexcept {
    return std::make_unique<DecalInstance>(*this);
}

DecalInstance::DecalInstance(const DecalInstance& rhs)
    : Instance {rhs}
    , model {rhs.model}
    , material {std::make_shared<ms::Material>(*rhs.material)} {
}
