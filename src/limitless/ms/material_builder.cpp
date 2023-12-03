#include <limitless/ms/material_builder.hpp>

#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/ms/material_compiler.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/assets.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/uniform/uniform_time.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>

using namespace Limitless::ms;

void Material::Builder::setMaterialIndex() {
    std::unique_lock lock(mutex);

    //TODO: make snippets comparisons
    if (!uniforms.empty() || !vertex_snippet.empty() || !fragment_snippet.empty() || !global_snippet.empty() || !shading_snippet.empty()) {
        // it is custom material; makes index unique
        shader_index = next_shader_index++;
    } else {
        const auto material_type = getMaterialType();
        if (auto found = unique_materials.find(material_type); found != unique_materials.end()) {
            // already exists
            shader_index = found->second;
        } else {
            // new one
            shader_index = next_shader_index++;
            unique_materials.emplace(material_type, shader_index);
        }
    }
}

void Material::Builder::checkRequirements() {
    if (_name.empty()) {
        throw material_builder_exception("Name cannot be empty!");
    }
}

std::shared_ptr<Material> Material::Builder::build(Assets& assets) {
    checkRequirements();

    setMaterialIndex();

    if (!_skybox) {
        setModelShaders();
    }

    auto material = std::shared_ptr<Material>(new Material(*this));

    assets.materials.add(material->name, material);

    return material;
}

UniqueMaterial Material::Builder::getMaterialType() const noexcept {
    std::set<Property> props;

    std::for_each(properties.begin(), properties.end(), [&] (auto& prop) {
        props.emplace(prop.first);
    });

    return { std::move(props), _shading };
}

void Material::Builder::setModelShaders() {
    if (_model_shaders.empty()) {
        _model_shaders.emplace(InstanceType::Model);
    }
}

Material::Builder &Material::Builder::color(const glm::vec4& color) noexcept {
    properties[Property::Color] = std::make_unique<UniformValue<glm::vec4>>("_material_color", color);
    return *this;
}

Material::Builder &Material::Builder::emissive_color(const glm::vec3& emissive_color) noexcept {
    properties[Property::EmissiveColor] = std::make_unique<UniformValue<glm::vec3>>("_material_emissive_color", emissive_color);
    return *this;
}

Material::Builder& Material::Builder::diffuse(const std::shared_ptr<Texture>& texture) noexcept {
    properties[Property::Diffuse] = std::make_unique<UniformSampler>("_material_diffuse_texture", texture);
    return *this;
}

Material::Builder &Material::Builder::normal(const std::shared_ptr<Texture>& texture) noexcept {
    properties[Property::Normal] = std::make_unique<UniformSampler>("_material_normal_texture", texture);
    return *this;
}

Material::Builder &Material::Builder::emissive_mask(const std::shared_ptr<Texture>& texture) noexcept {
    properties[Property::EmissiveMask] = std::make_unique<UniformSampler>("_material_emissive_mask_texture", texture);
    return *this;
}

Material::Builder &Material::Builder::blend_mask(const std::shared_ptr<Texture> &texture) noexcept {
    properties[Property::BlendMask] = std::make_unique<UniformSampler>("_material_blend_mask_texture", texture);
    return *this;
}

Material::Builder &Material::Builder::metallic(float metallic) noexcept {
    properties[Property::Metallic] = std::make_unique<UniformValue<float>>("_material_metallic", metallic);
    return *this;
}

Material::Builder &Material::Builder::metallic(const std::shared_ptr<Texture>& texture) noexcept {
    properties[Property::MetallicTexture] = std::make_unique<UniformSampler>("_material_metallic_texture", texture);
    return *this;
}

Material::Builder &Material::Builder::roughness(float roughness) noexcept {
    properties[Property::Roughness] = std::make_unique<UniformValue<float>>("_material_roughness", roughness);
    return *this;
}

Material::Builder &Material::Builder::roughness(const std::shared_ptr<Texture>& texture) noexcept {
    properties[Property::RoughnessTexture] = std::make_unique<UniformSampler>("_material_roughness_texture", texture);
    return *this;
}

Material::Builder &Material::Builder::ao(const std::shared_ptr<Texture>& texture) noexcept {
    properties[Property::AmbientOcclusionTexture] = std::make_unique<UniformSampler>("_material_ambient_occlusion_texture", texture);
    return *this;
}

Material::Builder &Material::Builder::orm(const std::shared_ptr<Texture>& texture) noexcept {
    properties[Property::ORM] = std::make_unique<UniformSampler>("_material_orm_texture", texture);
    return *this;
}

Material::Builder &Material::Builder::ior(float ior) noexcept {
    properties[Property::IoR] = std::make_unique<UniformValue<float>>("_material_ior", ior);
    return *this;
}

Material::Builder &Material::Builder::absorption(float absorption) noexcept {
    properties[Property::Absorption] = std::make_unique<UniformValue<float>>("_material_absorption", absorption);
    return *this;
}

Material::Builder &Material::Builder::microthickness(float microthickness) noexcept {
    properties[Property::MicroThickness] = std::make_unique<UniformValue<float>>("_material_microthickness", microthickness);
    return *this;
}

Material::Builder &Material::Builder::thickness(float thickness) noexcept {
    properties[Property::Thickness] = std::make_unique<UniformValue<float>>("_material_thickness", thickness);
    return *this;
}

Material::Builder &Material::Builder::reflectance(float reflectance) noexcept {
    properties[Property::Reflectance] = std::make_unique<UniformValue<float>>("_material_reflectance", reflectance);
    return *this;
}

Material::Builder &Material::Builder::transmission(float transmission) noexcept {
    properties[Property::Transmission] = std::make_unique<UniformValue<float>>("_material_transmission", transmission);
    return *this;
}

Material::Builder &Material::Builder::blending(Blending blending) noexcept {
    _blending = blending;
    return *this;
}

Material::Builder &Material::Builder::shading(Shading shading) noexcept {
    _shading = shading;
    return *this;
}

Material::Builder &Material::Builder::two_sided(bool two_sided) noexcept {
    _two_sided = two_sided;
    return *this;
}

Material::Builder &Material::Builder::name(const std::string& name) noexcept {
    _name = name;
    return *this;
}

Material::Builder &Material::Builder::vertex(const std::string& snippet) noexcept {
    vertex_snippet = snippet;
    return *this;
}

Material::Builder &Material::Builder::fragment(const std::string& snippet) noexcept {
    fragment_snippet = snippet;
    return *this;
}

Material::Builder &Material::Builder::global(const std::string& snippet) noexcept {
    global_snippet = snippet;
    return *this;
}

Material::Builder &Material::Builder::shading(const std::string& snippet) noexcept {
    shading_snippet = snippet;
    return *this;
}

Material::Builder &Material::Builder::model(InstanceType model) noexcept {
    _model_shaders = { model };
    return *this;
}

Material::Builder& Material::Builder::models(const InstanceTypes& models) noexcept {
    _model_shaders = models;
    return *this;
}

Material::Builder &Material::Builder::skybox() noexcept {
    _skybox = true;
    return *this;
}

Material::Builder& Material::Builder::time() noexcept {
    uniforms["time"] = std::make_unique<UniformTime>("time");
    return *this;
}

Material::Builder &Material::Builder::custom(const std::string& name, float value) noexcept {
    uniforms[name] = std::make_unique<UniformValue<float>>(name, value);
    return *this;
}

Material::Builder &Material::Builder::custom(const std::string &name, int32_t value) noexcept {
    uniforms[name] = std::make_unique<UniformValue<int32_t>>(name, value);
    return *this;
}

Material::Builder &Material::Builder::custom(const std::string &name, const glm::mat3 &value) noexcept {
    uniforms[name] = std::make_unique<UniformValue<glm::mat3>>(name, value);
    return *this;
}

Material::Builder &Material::Builder::custom(const std::string &name, const glm::mat4 &value) noexcept {
    uniforms[name] = std::make_unique<UniformValue<glm::mat4>>(name, value);
    return *this;
}

Material::Builder &Material::Builder::custom(const std::string &name, uint32_t value) noexcept {
    uniforms[name] = std::make_unique<UniformValue<uint32_t>>(name, value);
    return *this;
}

Material::Builder &Material::Builder::custom(const std::string &name, const glm::vec2 &value) noexcept {
    uniforms[name] = std::make_unique<UniformValue<glm::vec2>>(name, value);
    return *this;
}

Material::Builder &Material::Builder::custom(const std::string &name, const glm::vec3 &value) noexcept {
    uniforms[name] = std::make_unique<UniformValue<glm::vec3>>(name, value);
    return *this;
}

Material::Builder &Material::Builder::custom(const std::string &name, const glm::vec4 &value) noexcept {
    uniforms[name] = std::make_unique<UniformValue<glm::vec4>>(name, value);
    return *this;
}

Material::Builder &Material::Builder::custom(const std::string &name, const std::shared_ptr<Texture> &value) noexcept {
    uniforms[name] = std::make_unique<UniformSampler>(name, value);
    return *this;
}

Material::Builder &Material::Builder::refraction(bool refraction_) noexcept {
    _refraction = refraction_;
    return *this;
}
