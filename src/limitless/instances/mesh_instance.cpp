#include <limitless/instances/mesh_instance.hpp>

#include <limitless/models/mesh.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/core/context.hpp>

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
                        ShaderType pass,
                        InstanceType model,
                        const glm::mat4& model_matrix,
                        ms::Blending blending,
                        const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }
    //TODO: refactor this shitty unreadable nonhuman orc code
    if (!material.isLayered()) {
        const auto& mat = material[0];

        if (mat.getBlending() != blending) {
            return;
        }

        // sets state for material
        material.setMaterialState(ctx, 0, pass);

        // gets required shader from storage
        auto& shader = assets.shaders.get(pass, model, mat.getShaderIndex());

        // updates model/material uniforms
        shader.setUniform("_model_transform", model_matrix)
                .setMaterial(mat);

        // sets custom pass-dependent uniforms
        uniform_setter(shader);

        shader.use();

        mesh->draw();
        return;
    }

    // iterates over material layers
    for (const auto& [index, mat] : material) {
        if (mat->getBlending() != blending) {
            continue;
        }

        // sets state for material
        material.setMaterialState(ctx, index, pass);

        // gets required shader from storage
        auto& shader = assets.shaders.get(pass, model, mat->getShaderIndex());

        // updates model/material uniforms
        shader.setUniform("_model_transform", model_matrix)
              .setMaterial(*mat);

        // sets custom pass-dependent uniforms
        uniform_setter(shader);

        shader.use();

        mesh->draw();
    }
}

void MeshInstance::draw_instanced(Context& ctx,
                                  const Assets& assets,
                                  ShaderType pass,
                                  InstanceType model,
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
        material.setMaterialState(ctx, index, pass);

        // gets required shader from storage
        auto& shader = assets.shaders.get(pass, model, mat->getShaderIndex());

        // updates model/material uniforms
        shader.setUniform("_model_transform", model_matrix)
              .setMaterial(*mat);

        // sets custom pass-dependent uniforms
        uniform_setter(shader);

        shader.use();

        mesh->draw_instanced(count);
    }
}

void MeshInstance::update() {
	for (const auto& [_, mat] : material) {
		mat->update();
	}
}
