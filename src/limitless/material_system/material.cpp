#include <limitless/material_system/material.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/bindless_texture.hpp>
#include <limitless/core/texture.hpp>
#include <cstring>

#include <limitless/core/buffer_builder.hpp>

using namespace LimitlessEngine;

void Material::update() const noexcept {
    // checks if there are values need to be mapped into material buffer
    bool changed = false;
    for (const auto& [type, uniform] : properties) {
        if (uniform->getChanged()) {
            changed = true;
            break;
        }
    }

    if (!changed) return;

    std::vector<std::byte> data(material_buffer->getSize());
    for (const auto& [type, prop] : properties) {
        switch (type) {
            case PropertyType::Color:
            case PropertyType::EmissiveColor: {
                auto& uniform = static_cast<UniformValue<glm::vec4>&>(*properties.at(type));
                auto offset = uniform_offsets.at(uniform.getName());
                std::memcpy(data.data() + offset, &uniform.getValue(), sizeof(glm::vec4));
                break;
            }
            case PropertyType::Shininess:
            case PropertyType::Metallic:
            case PropertyType::Roughness: {
                auto& uniform = static_cast<UniformValue<float> &>(*properties.at(type));
                auto offset = uniform_offsets.at(uniform.getName());
                std::memcpy(data.data() + offset, &uniform.getValue(), sizeof(float));
                break;
            }
            case PropertyType::Diffuse:
            case PropertyType::Specular:
            case PropertyType::Normal:
            case PropertyType::Displacement:
            case PropertyType::EmissiveMask:
            case PropertyType::BlendMask:
            case PropertyType::MetallicTexture:
            case PropertyType::RoughnessTexture:
                //TODO: based on real-time type
                if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                    auto& uniform = static_cast<UniformSampler&>(*properties.at(type));
                    auto offset = uniform_offsets.at(uniform.getName());
                    auto& texture = uniform.getSampler();
                    auto& bindless = static_cast<BindlessTexture&>(texture->getExtensionTexture());
                    bindless.makeResident();
                    std::memcpy(data.data() + offset, &bindless.getHandle(), sizeof(uint64_t));
                }
                break;
        }
    }

    material_buffer->mapData(data.data(), data.size());

    for (const auto& [type, uniform] : properties) {
        uniform->getChanged() = false;
    }
}

UniformValue<glm::vec4>& Material::getColor() const {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(PropertyType::Color));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No color in material.");
    }
}

UniformValue<glm::vec4>& Material::getEmissiveColor() const {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(PropertyType::EmissiveColor));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No emissive color in material.");
    }
}

UniformValue<float>& Material::getShininess() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(PropertyType::Shininess));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No shininess in material.");
    }
}

UniformValue<float>& Material::getMetallic() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(PropertyType::Metallic));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No metallic in material.");
    }
}

UniformValue<float>& Material::getRoughness() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(PropertyType::Roughness));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No roughness in material.");
    }
}

UniformSampler& Material::getDiffuse() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(PropertyType::Diffuse));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No diffuse in material.");
    }
}

UniformSampler& Material::getSpecular() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(PropertyType::Specular));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No specular in material.");
    }
}

UniformSampler& Material::getNormal() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(PropertyType::Normal));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No normal in material.");
    }
}

UniformSampler& Material::getDisplacement() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(PropertyType::Displacement));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No displacement in material.");
    }
}

UniformSampler& Material::getEmissiveMask() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(PropertyType::EmissiveMask));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No emissive mask in material.");
    }
}

UniformSampler& Material::getBlendMask() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(PropertyType::BlendMask));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No blend mask in material.");
    }
}

Material::Material(const Material& material) noexcept
    : uniform_offsets{material.uniform_offsets}, blending{material.blending}, shading{material.shading}, name{material.name}, shader_index{material.shader_index}, two_sided{material.two_sided} {
    for (const auto& [type, property] : material.properties) {
        properties.emplace(type, property->clone());
    }

    BufferBuilder builder;
    material_buffer = builder.setTarget(Buffer::Type::Uniform)
                             .setUsage(Buffer::Usage::DynamicDraw)
                             .setAccess(Buffer::MutableAccess::WriteOrphaning)
                             .setDataSize(material.material_buffer->getSize())
                             .build();
}

void LimitlessEngine::swap(Material& lhs, Material& rhs) noexcept {
    using std::swap;

    swap(lhs.properties, rhs.properties);
    swap(lhs.uniform_offsets, rhs.uniform_offsets);
    swap(lhs.material_buffer, rhs.material_buffer);
    swap(lhs.blending, rhs.blending);
    swap(lhs.shading, rhs.shading);
    swap(lhs.name, rhs.name);
    swap(lhs.shader_index, rhs.shader_index);
    swap(lhs.two_sided, rhs.two_sided);
}

Material &Material::operator=(Material material) noexcept {
    swap(*this, material);
    return *this;
}

Material* Material::clone() const noexcept {
    return new Material(*this);
}

UniformSampler& Material::getMetallicTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(PropertyType::MetallicTexture));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No MetallicTexture in material.");
    }
}

UniformSampler& Material::getRoughnessTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(PropertyType::RoughnessTexture));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No RoughnessTexture in material.");
    }
}

Material::Material(Material&& rhs) noexcept
    : Material{} {
    swap(*this, rhs);
}

Material& Material::operator=(Material&& rhs) noexcept {
    swap(*this, rhs);
    return *this;
}

bool LimitlessEngine::operator<(const MaterialType& lhs, const MaterialType& rhs) noexcept {
    return std::tie(lhs.properties, lhs.shading) < std::tie(rhs.properties, rhs.shading);
}
