#include <limitless/ms/material.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/bindless_texture.hpp>
#include <limitless/core/buffer_builder.hpp>
#include <limitless/core/texture.hpp>
#include <cstring>
#include <iostream>

using namespace Limitless::ms;
using namespace Limitless;

void Limitless::ms::swap(Material& lhs, Material& rhs) noexcept {
    using std::swap;

    swap(lhs.properties, rhs.properties);
    swap(lhs.blending, rhs.blending);
    swap(lhs.shading, rhs.shading);
    swap(lhs.two_sided, rhs.two_sided);
    swap(lhs.name, rhs.name);
    swap(lhs.shader_index, rhs.shader_index);
    swap(lhs.model_shaders, rhs.model_shaders);
    swap(lhs.material_buffer, rhs.material_buffer);
    swap(lhs.uniform_offsets, rhs.uniform_offsets);
    swap(lhs.uniforms, rhs.uniforms);
    swap(lhs.vertex_snippet, rhs.vertex_snippet);
    swap(lhs.fragment_snippet, rhs.fragment_snippet);
    swap(lhs.global_snippet, rhs.global_snippet);
}

Material& Material::operator=(Material material) {
    swap(*this, material);
    return *this;
}

Material::Material(const Material& material)
    : blending {material.blending}
    , shading {material.shading}
    , two_sided {material.two_sided}
    , name {material.name}
    , shader_index {material.shader_index}
    , model_shaders {material.model_shaders}
    , uniform_offsets {material.uniform_offsets}
    , vertex_snippet {material.vertex_snippet}
    , fragment_snippet {material.fragment_snippet}
    , global_snippet {material.global_snippet} {

    // deep copy of properties
    for (const auto& [type, property] : material.properties) {
        properties.emplace(type, property->clone());
    }

    // deep copy of custom properties
    for (const auto& [name, uniform] : material.uniforms) {
        uniforms.emplace(name, uniform->clone());
    }

    // TODO: empty copy function
    // copy of opengl buffer
    BufferBuilder builder;
    material_buffer = builder.setTarget(Buffer::Type::Uniform)
                             .setUsage(Buffer::Usage::DynamicDraw)
                             .setAccess(Buffer::MutableAccess::WriteOrphaning)
                             .setDataSize(material.material_buffer->getSize())
                             .build();
}

template<typename V>
void Material::map(std::vector<std::byte>& block, const Uniform& uniform) const {
    const auto& uni = static_cast<const UniformValue<V>&>(uniform);
    const auto offset = uniform_offsets.at(uniform.getName());
    std::memcpy(block.data() + offset, &uni.getValue(), sizeof(V));
}

void Material::map(std::vector<std::byte>& block, Uniform& uniform) {
    switch (uniform.getType()) {
        case UniformType::Value:
            switch (uniform.getValueType()) {
                case UniformValueType::Uint:
                    map<unsigned int>(block, uniform);
                    break;
                case UniformValueType::Int:
                    map<int>(block, uniform);
                    break;
                case UniformValueType::Float:
                    map<float>(block, uniform);
                    break;
                case UniformValueType::Vec2:
                    map<glm::vec2>(block, uniform);
                    break;
                case UniformValueType::Vec3:
                    map<glm::vec3>(block, uniform);
                    break;
                case UniformValueType::Vec4:
                    map<glm::vec4>(block, uniform);
                    break;
                case UniformValueType::Mat4:
                    map<glm::mat4>(block, uniform);
                    break;
                case UniformValueType::Mat3:
                    map<glm::mat3>(block, uniform);
                    break;
            }
            break;
        case UniformType::Sampler:
            if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                const auto& uni = static_cast<const UniformSampler&>(uniform);
                const auto offset = uniform_offsets.at(uniform.getName());
                auto& bindless_texture = static_cast<BindlessTexture&>(uni.getSampler()->getExtensionTexture());
                bindless_texture.makeResident();
                std::memcpy(block.data() + offset, &bindless_texture.getHandle(), sizeof(uint64_t));
            }
            break;
        case UniformType::Time: {
            auto& time = static_cast<UniformTime&>(uniform);
            time.update();
            map<float>(block, uniform);
            break;
        }
    }
}

void Material::map() {
    std::vector<std::byte> block(material_buffer->getSize());

    for (const auto& [property, uniform] : properties) {
        map(block, *uniform);
    }

    for (const auto& [name, uniform] : uniforms) {
        map(block, *uniform);
    }

    material_buffer->mapData(block.data(), block.size());
}

void Material::update() {
    const auto properties_changed = std::any_of(properties.begin(), properties.end(), [] (auto& property) { return property.second->getChanged(); });
    const auto uniforms_changed = std::any_of(uniforms.begin(), uniforms.end(), [] (auto& uniform) { return uniform.second->getChanged(); });

    if (!properties_changed && !uniforms_changed) {
        return;
    }

    map();

    for (const auto& [type, uniform] : properties) {
        uniform->getChanged() = false;
    }

    for (const auto& [name, uniform] : uniforms) {
        uniform->getChanged() = false;
    }
}

const UniformValue<glm::vec4>& Material::getColor() const {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::Color));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No color in material.");
    }
}

const UniformValue<glm::vec4>& Material::getEmissiveColor() const {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::EmissiveColor));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No emissive color in material.");
    }
}

const UniformValue<float>& Material::getShininess() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Shininess));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No shininess in material.");
    }
}

const UniformValue<float>& Material::getMetallic() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Metallic));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No metallic in material.");
    }
}

const UniformValue<float>& Material::getRoughness() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Roughness));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No roughness in material.");
    }
}

const UniformSampler& Material::getDiffuse() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Diffuse));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No diffuse in material.");
    }
}

const UniformSampler& Material::getSpecular() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Specular));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No specular in material.");
    }
}

const UniformSampler& Material::getNormal() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Normal));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No normal in material.");
    }
}

const UniformSampler& Material::getDisplacement() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Displacement));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No displacement in material.");
    }
}

const UniformSampler& Material::getEmissiveMask() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::EmissiveMask));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No emissive mask in material.");
    }
}

const UniformSampler& Material::getBlendMask() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::BlendMask));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No blend mask in material.");
    }
}

const UniformSampler& Material::getMetallicTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::MetallicTexture));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No MetallicTexture in material.");
    }
}

const UniformSampler& Material::getRoughnessTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::RoughnessTexture));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No RoughnessTexture in material.");
    }
}

const UniformValue<glm::vec2>& Material::getTesselationFactor() const {
    try {
        return static_cast<UniformValue<glm::vec2>&>(*properties.at(Property::TessellationFactor));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No RoughnessTexture in material.");
    }
}

UniformValue<glm::vec4>& Material::getColor() {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::Color));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No color in material.");
    }
}

UniformValue<glm::vec4>& Material::getEmissiveColor() {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::EmissiveColor));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No emissive color in material.");
    }
}

UniformValue<glm::vec2>& Material::getTesselationFactor() {
    try {
        return static_cast<UniformValue<glm::vec2>&>(*properties.at(Property::TessellationFactor));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No RoughnessTexture in material.");
    }
}

UniformValue<float>& Material::getShininess() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Shininess));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No shininess in material.");
    }
}

UniformValue<float>& Material::getMetallic() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Metallic));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No metallic in material.");
    }
}

UniformValue<float>& Material::getRoughness() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Roughness));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No roughness in material.");
    }
}

UniformSampler& Material::getMetallicTexture() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::MetallicTexture));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No MetallicTexture in material.");
    }
}

UniformSampler& Material::getRoughnessTexture() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::RoughnessTexture));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No RoughnessTexture in material.");
    }
}

UniformSampler& Material::getDiffuse() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Diffuse));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No diffuse in material.");
    }
}

UniformSampler& Material::getSpecular() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Specular));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No specular in material.");
    }
}

UniformSampler& Material::getNormal() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Normal));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No normal in material.");
    }
}

UniformSampler& Material::getDisplacement() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Displacement));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No displacement in material.");
    }
}

UniformSampler& Material::getEmissiveMask() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::EmissiveMask));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No emissive mask in material.");
    }
}

UniformSampler& Material::getBlendMask() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::BlendMask));
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No blend mask in material.");
    }
}

bool Material::contains(Property property) const {
    return properties.count(property);
}

bool Material::contains(const std::string& uniform_name) const {
    return uniforms.count(uniform_name);
}
