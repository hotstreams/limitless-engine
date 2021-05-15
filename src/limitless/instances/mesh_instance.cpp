#include <limitless/instances/mesh_instance.hpp>

#include <limitless/models/mesh.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader_program.hpp>

using namespace Limitless;

MeshInstance::MeshInstance(std::shared_ptr<AbstractMesh> _mesh, const std::shared_ptr<ms::Material>& _material) noexcept
    : mesh {std::move(_mesh)}
    , material {_material} {
}

void MeshInstance::hide() noexcept {
    hidden = true;
}

void MeshInstance::reveal() noexcept {
    hidden = false;
}

void MeshInstance::draw(Context& ctx,
                        const Assets& assets,
                        ShaderPass pass,
                        ModelShader model,
                        const glm::mat4& model_matrix,
                        ms::Blending blending,
                        const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    // iterates over material layers
    for (const auto& [index, mat] : material) {
        if (mat->getBlending() != blending) {
            continue;
        }

        // sets state for material
        material.setMaterialState(ctx, index);

        // gets required shader from storage
        auto& shader = assets.shaders.get(pass, model, mat->getShaderIndex());

        // updates model/material uniforms
        shader << UniformValue {"model", model_matrix}
               << *mat;

        // sets custom pass-dependent uniforms
        uniform_setter(shader);

        shader.use();

        const auto draw_mode = mat->contains(ms::Property::TessellationFactor) ? DrawMode::Patches : mesh->getDrawMode();

        mesh->draw(draw_mode);
    }
}

void MeshInstance::draw_instanced(Context& ctx,
                        const Assets& assets,
                        ShaderPass pass,
                        ModelShader model,
                        const glm::mat4& model_matrix,
                        ms::Blending blending,
                        const UniformSetter& uniform_setter,
                        uint32_t count) {
    if (hidden) {
        return;
    }

    // iterates over material layers
    for (const auto& [index, mat] : material) {
        if (mat->getBlending() != blending) {
            continue;
        }

        // sets state for material
        material.setMaterialState(ctx, index);

        // gets required shader from storage
        auto& shader = assets.shaders.get(pass, model, mat->getShaderIndex());

        // updates model/material uniforms
        shader << UniformValue {"model", model_matrix}
               << *mat;

        // sets custom pass-dependent uniforms
        uniform_setter(shader);

        shader.use();

        const auto draw_mode = mat->contains(ms::Property::TessellationFactor) ? DrawMode::Patches : mesh->getDrawMode();

        mesh->draw_instanced(draw_mode, count);
    }
}
