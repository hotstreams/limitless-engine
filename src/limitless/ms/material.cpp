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
    swap(lhs.shading_snippet, rhs.shading_snippet);
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

const glm::vec4& Material::getColor() const {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::Color)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Color");
    }
}

const glm::vec4& Material::getEmissiveColor() const {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::EmissiveColor)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Emissive Color");
    }
}

float Material::getMetallic() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Metallic)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Metallic");
    }
}

float Material::getRoughness() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Roughness)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Roughness");
    }
}

const std::shared_ptr<Texture>& Material::getDiffuseTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Diffuse)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Diffuse");
    }
}

const std::shared_ptr<Texture>& Material::getNormalTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Normal)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Normal");
    }
}

const std::shared_ptr<Texture>& Material::getEmissiveMaskTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::EmissiveMask)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - EmissiveMask");
    }
}

const std::shared_ptr<Texture>& Material::getBlendMaskTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::BlendMask)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - BlendMask");
    }
}

const std::shared_ptr<Texture>& Material::getMetallicTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::MetallicTexture)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - MetallicTexture");
    }
}

const std::shared_ptr<Texture>& Material::getRoughnessTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::RoughnessTexture)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - RoughnessTexture");
    }
}

glm::vec4& Material::getColor() {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::Color)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Color");
    }
}

glm::vec4& Material::getEmissiveColor() {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::EmissiveColor)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - EmissiveColor");
    }
}

float& Material::getMetallic() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Metallic)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Metallic");
    }
}

float& Material::getRoughness() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Roughness)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Roughness");
    }
}

float& Material::getAbsorption() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Absorption)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Absorption");
    }
}

bool Material::contains(Property property) const {
    return properties.count(property);
}

bool Material::contains(const std::string& uniform_name) const {
    return uniforms.count(uniform_name);
}

float& Material::getIoR() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::IoR)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - IoR");
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

const UniformSampler &Material::getORMTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::ORM)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No ORM in material.");
    }
}

UniformSampler &Material::getORMTexture() {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::ORM)); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_property_not_found("No ORM in material.");
    }
}
