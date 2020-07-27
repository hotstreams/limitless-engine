#include <custom_material.hpp>

using namespace GraphicsEngine;

void CustomMaterial::update() const noexcept {
    // checks if there are values need to be mapped into material buffer
    bool changed = false;
    for (const auto& [type, uniform] : properties) {
        if (uniform->getChanged()) {
            changed = true;
            break;
        }
    }

    for (const auto& [name, uniform] : uniforms) {
        if (uniform->getChanged()) {
            changed = true;
            break;
        }
    }

    if (!changed) return;

    std::vector<std::byte> data(material_buffer->getSize());

    for (const auto& [name, offset] : uniform_offsets) {

    }

    material_buffer->mapData(data.data(), data.size());

    for (const auto& [type, uniform] : properties) {
        uniform->getChanged() = false;
    }
}

CustomMaterial::CustomMaterial(decltype(properties)&& properties, decltype(uniform_offsets)&& offsets, Blending blending,
                               Shading shading, std::string name, uint64_t shader_index, decltype(uniforms)&& uniforms) noexcept
    : Material(std::move(properties), std::move(offsets), blending, shading, std::move(name), shader_index), uniforms{std::move(uniforms)}
{

}

void GraphicsEngine::swap(CustomMaterial& lhs, CustomMaterial& rhs) noexcept {
    using std::swap;

    swap(static_cast<Material&>(lhs), static_cast<Material&>(rhs));

    swap(lhs.uniforms, rhs.uniforms);
}

CustomMaterial::CustomMaterial(const CustomMaterial& material)
    : Material(material) {
    for (const auto& [name, uniform] : uniforms) {
        uniforms.emplace(name, uniform->clone());
    }
}

CustomMaterial& CustomMaterial::operator=(CustomMaterial material) {
    swap(*this, material);

    return *this;
}

Material* CustomMaterial::clone() const {
    return new CustomMaterial(*this);
}

