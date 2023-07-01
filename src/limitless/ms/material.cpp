#include <limitless/ms/material.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/uniform/uniform_time.hpp>
#include <limitless/core/texture/bindless_texture.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/texture/texture.hpp>

#include <cstring>

using namespace Limitless::ms;
using namespace Limitless;

void Limitless::ms::swap(Material& lhs, Material& rhs) noexcept {
    using std::swap;

    swap(lhs.properties, rhs.properties);
    swap(lhs.blending, rhs.blending);
    swap(lhs.shading, rhs.shading);
    swap(lhs.two_sided, rhs.two_sided);
    swap(lhs.refraction, rhs.refraction);
    swap(lhs.name, rhs.name);
    swap(lhs.shader_index, rhs.shader_index);
    swap(lhs.model_shaders, rhs.model_shaders);
    swap(lhs.material_buffer, rhs.material_buffer);
    swap(lhs.uniform_offsets, rhs.uniform_offsets);
    swap(lhs.uniforms, rhs.uniforms);
    swap(lhs.vertex_snippet, rhs.vertex_snippet);
    swap(lhs.fragment_snippet, rhs.fragment_snippet);
    swap(lhs.global_snippet, rhs.global_snippet);
    swap(lhs.tessellation_snippet, rhs.tessellation_snippet);
}

Material& Material::operator=(Material material) {
    swap(*this, material);
    return *this;
}

bool Limitless::ms::operator==(const Material& lhs, const Material& rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool Limitless::ms::operator<(const Material& lhs, const Material& rhs) noexcept {
    {
        if (lhs.properties.size() != rhs.properties.size()) {
            return lhs.properties.size() < rhs.properties.size();
        }

        auto lhs_it = lhs.properties.begin();
        auto rhs_it = rhs.properties.begin();

        while (lhs_it != lhs.properties.end()) {
            if (lhs_it->first != rhs_it->first) {
                return lhs_it->first < rhs_it->first;
            }

            if (*lhs_it->second != *rhs_it->second) {
                return *lhs_it->second < *rhs_it->second;
            }

            lhs_it++;
            rhs_it++;
        }
    }

    {
        if (lhs.uniforms.size() != rhs.uniforms.size()) {
            return lhs.uniforms.size() < rhs.uniforms.size();
        }

        auto lhs_it = lhs.uniforms.begin();
        auto rhs_it = rhs.uniforms.begin();

        while (lhs_it != lhs.uniforms.end()) {
            if (lhs_it->first != rhs_it->first) {
                return lhs_it->first < rhs_it->first;
            }

            if (*lhs_it->second != *rhs_it->second) {
                return *lhs_it->second < *rhs_it->second;
            }

            lhs_it++;
            rhs_it++;
        }
    }

    return std::tie(lhs.blending, lhs.two_sided, lhs.shader_index) < std::tie(rhs.blending, rhs.two_sided, rhs.shader_index);
}

Material::Material(const Material& material)
    : blending {material.blending}
    , shading {material.shading}
    , two_sided {material.two_sided}
    , refraction {material.refraction}
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
    for (const auto& [uniform_name, uniform] : material.uniforms) {
        uniforms.emplace(uniform_name, uniform->clone());
    }

    material_buffer = std::shared_ptr<Buffer>(material.material_buffer->clone());

    //TODO: what to do with copy? map here or in update?
    // move to update mb
    map();
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
            if (ContextInitializer::isBindlessTexturesSupported()) {
                const auto& uni = static_cast<const UniformSampler&>(uniform); //NOLINT
                const auto offset = uniform_offsets.at(uniform.getName());
                auto& bindless_texture = static_cast<BindlessTexture&>(uni.getSampler()->getExtensionTexture()); //NOLINT
                bindless_texture.makeResident();
                std::memcpy(block.data() + offset, &bindless_texture.getHandle(), sizeof(uint64_t));
            }
            break;
        case UniformType::Time: {
            auto& time = static_cast<UniformTime&>(uniform); //NOLINT
            time.update();
            map<float>(block, uniform);
            break;
        }
    }
}

void Material::map() {
    std::vector<std::byte> block(material_buffer->getSize());

    for (const auto& [_, uniform] : properties) {
        map(block, *uniform);
    }

    for (const auto& [_, uniform] : uniforms) {
        map(block, *uniform);
    }

    std::memcpy(block.data() + block.size() - 4, &shading, 4);

    material_buffer->mapData(block.data(), block.size());
}

void Material::update() {
    //TODO: not map the whole buffer if changed only 1 value?
    // must do for time
    const auto properties_changed = std::any_of(properties.begin(), properties.end(), [] (auto& property) {
        return property.second->isChanged();
    });
    const auto uniforms_changed = std::any_of(uniforms.begin(), uniforms.end(), [] (auto& uniform) {
        return uniform.second->isChanged() || uniform.second->getType() == UniformType::Time;
    });

    if (!properties_changed && !uniforms_changed) {
        return;
    }

    map();

    for (const auto& [_, uniform] : properties) {
        uniform->resetChanged();
    }

    for (const auto& [_, uniform] : uniforms) {
        uniform->resetChanged();
    }
}

const UniformValue<glm::vec4>& Material::getColor() const {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::Color)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No color in material.");
    }
}

const UniformValue<glm::vec4>& Material::getEmissiveColor() const {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::EmissiveColor)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No emissive color in material.");
    }
}

const UniformValue<float>& Material::getMetallic() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Metallic)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No metallic in material.");
    }
}

const UniformValue<float>& Material::getRoughness() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Roughness)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No roughness in material.");
    }
}

const UniformSampler& Material::getDiffuse() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Diffuse)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No diffuse in material.");
    }
}

const UniformSampler& Material::getNormal() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Normal)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No normal in material.");
    }
}

const UniformSampler& Material::getEmissiveMask() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::EmissiveMask)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No emissive mask in material.");
    }
}

const UniformSampler& Material::getBlendMask() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::BlendMask)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No blend mask in material.");
    }
}

const UniformSampler& Material::getMetallicTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::MetallicTexture)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No MetallicTexture in material.");
    }
}

const UniformSampler& Material::getRoughnessTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::RoughnessTexture)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No RoughnessTexture in material.");
    }
}

const UniformValue<glm::vec2>& Material::getTesselationFactor() const {
    try {
        return static_cast<UniformValue<glm::vec2>&>(*properties.at(Property::TessellationFactor)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No RoughnessTexture in material.");
    }
}

UniformValue<glm::vec4>& Material::getColor() {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::Color)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No color in material.");
    }
}

UniformValue<glm::vec4>& Material::getEmissiveColor() {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::EmissiveColor)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No emissive color in material.");
    }
}

UniformValue<glm::vec2>& Material::getTesselationFactor() {
    try {
        return static_cast<UniformValue<glm::vec2>&>(*properties.at(Property::TessellationFactor)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No RoughnessTexture in material.");
    }
}

UniformValue<float>& Material::getMetallic() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Metallic)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No metallic in material.");
    }
}

UniformValue<float>& Material::getRoughness() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Roughness)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No roughness in material.");
    }
}

UniformSampler& Material::getMetallicTexture() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::MetallicTexture)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No MetallicTexture in material.");
    }
}

UniformSampler& Material::getRoughnessTexture() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::RoughnessTexture)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No RoughnessTexture in material.");
    }
}

UniformSampler& Material::getDiffuse() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Diffuse)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No diffuse in material.");
    }
}

UniformSampler& Material::getNormal() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Normal)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No normal in material.");
    }
}

UniformSampler& Material::getEmissiveMask() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::EmissiveMask)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No emissive mask in material.");
    }
}

UniformSampler& Material::getBlendMask() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::BlendMask)); //NOLINT
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

const UniformValue<float>& Material::getAbsorption() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Absorption)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No absorption in material.");
    }
}

UniformValue<float>& Material::getIoR() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::IoR)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No IoR in material.");
    }
}

UniformValue<float>& Material::getAbsorption() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Absorption)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No absorption in material.");
    }
}

const UniformValue<float>& Material::getIoR() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::IoR)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No IoR in material.");
    }
}

const UniformSampler &Material::getAmbientOcclusionTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::AmbientOcclusionTexture)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No AmbientOcclusionTexture in material.");
    }
}

UniformSampler &Material::getAmbientOcclusionTexture() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::AmbientOcclusionTexture)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No AmbientOcclusionTexture in material.");
    }
}
