#include <limitless/material_system/custom_material_builder.hpp>

using namespace LimitlessEngine;

CustomMaterialBuilder& CustomMaterialBuilder::addUniform(std::string name, Uniform* uniform) {
    auto result = static_cast<CustomMaterial&>(*material).uniforms.emplace(std::move(name), uniform);
    if (!result.second) {
        throw material_builder_error{"Failed to add uniform to custom material, already contains."};
    }
    return *this;
}

void CustomMaterialBuilder::setMaterialIndex() {
    std::unique_lock lock(mutex);

    // every custom material is unique
    material->shader_index = next_shader_index++;
}

void CustomMaterialBuilder::checkRequirements() {
    MaterialBuilder::checkRequirements();

    const auto& custom = static_cast<CustomMaterial&>(*material);

    if (custom.uniforms.empty() && custom.vertex_code.empty() && custom.fragment_code.empty()) {
        throw material_builder_error{"Uniform, vertex code, fragment code cannot be empty."};
    }
}

CustomMaterialBuilder& CustomMaterialBuilder::setVertexCode(std::string vs_code) noexcept {
    static_cast<CustomMaterial&>(*material).vertex_code = std::move(vs_code);
    return *this;
}

CustomMaterialBuilder& CustomMaterialBuilder::setFragmentCode(std::string fs_code) noexcept {
    static_cast<CustomMaterial&>(*material).fragment_code = std::move(fs_code);
    return *this;
}

CustomMaterialBuilder& CustomMaterialBuilder::create(std::string name) {
    material = std::shared_ptr<Material>(new CustomMaterial());
    material->name = std::move(name);
    return *this;
}

CustomMaterialBuilder& CustomMaterialBuilder::setUniforms(decltype(CustomMaterial::uniforms)&& uniforms) {
    static_cast<CustomMaterial&>(*material).uniforms = std::move(uniforms);
    return *this;
}

CustomMaterialBuilder& CustomMaterialBuilder::removeUniform(const std::string& name) {
    static_cast<CustomMaterial&>(*material).uniforms.erase(name);
    return *this;
}
