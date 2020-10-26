#include <material_system/custom_material_builder.hpp>
#include <core/shader_compiler.hpp>
#include <assets.hpp>
#include <material_system/material_compiler.hpp>
#include <shader_storage.hpp>

using namespace GraphicsEngine;

CustomMaterialBuilder& CustomMaterialBuilder::addUniform(std::string name, Uniform* uniform) {
    auto result = material->uniforms.emplace(std::move(name), uniform);
    if (!result.second) {
        throw std::runtime_error("Failed to add uniform to custom material, already exists.");
    }
    return *this;
}

CustomMaterialBuilder& CustomMaterialBuilder::setVertexCode(std::string vs_code) noexcept {
    material->vertex_code = std::move(vs_code);
    return *this;
}

CustomMaterialBuilder& CustomMaterialBuilder::setFragmentCode(std::string fs_code) noexcept {
    material->fragment_code = std::move(fs_code);
    return *this;
}

std::shared_ptr<Material> CustomMaterialBuilder::build(const ModelShaders& model_shaders, const MaterialShaders& material_shaders) {
    // check for initialized ptr
    if (!material) {
        throw std::runtime_error("Forgot to call create(name)!");
    }
    // checks for empty buffer
    if (material->properties.empty() && material->uniforms.empty()) {
        throw std::runtime_error("Properties & Uniforms cannot be empty.");
    }

    // every custom material is unique
    material->shader_index = next_shader_index++;

    // compiles every material/shader combination
    MaterialCompiler compiler;
    for (const auto& mat_shader : material_shaders) {
        for (const auto& mod_shader : model_shaders) {
            compiler.compile(*material, mat_shader, mod_shader);
        }
    }

    // initializes uniform material buffer using program shader introspection
    initializeMaterialBuffer(*shader_storage.get(material_shaders[0], model_shaders[0], material->shader_index));

    // adds compiled material to assets
    auto compiled = std::shared_ptr<Material>(std::move(material));
    assets.materials.add(compiled->getName(), compiled);

    return compiled;
}

CustomMaterialBuilder& CustomMaterialBuilder::create(std::string name) {
    material = std::unique_ptr<CustomMaterial>(new CustomMaterial());
    material->name = std::move(name);
    return *this;
}
