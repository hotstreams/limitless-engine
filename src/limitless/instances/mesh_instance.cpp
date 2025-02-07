#include <limitless/instances/mesh_instance.hpp>

#include <limitless/models/mesh.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/core/context.hpp>

using namespace Limitless;
using namespace Limitless::ms;

MeshInstance::MeshInstance(std::shared_ptr<Mesh> mesh, const std::shared_ptr<ms::Material>& material) noexcept
    : mesh {std::move(mesh)}
    , material {std::make_shared<Material>(*material)} {
}

MeshInstance::MeshInstance(const MeshInstance& rhs)
    : mesh {rhs.mesh}
    , material {std::make_shared<Material>(*rhs.material)}
    , base {std::make_shared<Material>(*rhs.base)} {
}

void MeshInstance::changeBaseMaterial(const std::shared_ptr<ms::Material>& material_) noexcept {
    base = std::make_shared<Material>(*material_);
    material = std::make_shared<Material>(*material_);
}

void MeshInstance::changeMaterial(const std::shared_ptr<Material>& material_) noexcept {
    material = std::make_shared<Material>(*material_);
}

void MeshInstance::reset() noexcept {
    material = base;
}

//void MeshInstance::draw(Context& ctx,
//                        const Assets& assets,
//                        ShaderType pass,
//                        InstanceType model,
//                        const glm::mat4& model_matrix,
//                        ms::Blending blending,
//                        const UniformSetter& uniform_setter) {
//
//    if (material->getBlending() != blending) {
//        return;
//    }
//
//    setBlendingMode(material->getBlending());
//
//    // sets culling based on two-sideness
//    if (material->getTwoSided()) {
//        ctx.disable(Capabilities::CullFace);
//    } else {
//        ctx.enable(Capabilities::CullFace);
//
//        // front cullfacing for shadows helps prevent peter panning
//        if (pass == ShaderType::DirectionalShadow) {
//            ctx.setCullFace(CullFace::Front);
//        } else {
//            ctx.setCullFace(CullFace::Back);
//        }
//    }
//
//    // gets required shader from storage
//    auto& shader = assets.shaders.get(pass, model, material->getShaderIndex());
//
//    // updates model/material uniforms
//    shader.setUniform("_model_transform", model_matrix)
//          .setMaterial(*material);
//
//    // sets custom pass-dependent uniforms
//    uniform_setter(shader);
//
//    shader.use();
//
//    mesh->draw();
//}
//
//void MeshInstance::draw_instanced(Context& ctx,
//                                  const Assets& assets,
//                                  ShaderType pass,
//                                  InstanceType model,
//                                  const glm::mat4& model_matrix,
//                                  ms::Blending blending,
//                                  const UniformSetter& uniform_setter,
//                                  uint32_t count) {
//    if (material->getBlending() != blending) {
//        return;
//    }
//
//    setBlendingMode(material->getBlending());
//
//    // sets culling based on two-sideness
//    if (material->getTwoSided()) {
//        ctx.disable(Capabilities::CullFace);
//    } else {
//        ctx.enable(Capabilities::CullFace);
//
//        // front cullfacing for shadows helps prevent peter panning
//        if (pass == ShaderType::DirectionalShadow) {
//            ctx.setCullFace(CullFace::Front);
//        } else {
//            ctx.setCullFace(CullFace::Back);
//        }
//    }
//
//    // gets required shader from storage
//    auto& shader = assets.shaders.get(pass, model, material->getShaderIndex());
//
//    // updates model/material uniforms
//    shader.setUniform("_model_transform", model_matrix)
//            .setMaterial(*material);
//
//    // sets custom pass-dependent uniforms
//    uniform_setter(shader);
//
//    shader.use();
//
//    mesh->draw_instanced(count);
//}

void MeshInstance::update() {
    material->update();
}
