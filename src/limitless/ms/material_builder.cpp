#include <limitless/ms/material_builder.hpp>

#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/ms/material_compiler.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/assets.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/uniform/uniform_time.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/uniform/uniform_value_array.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/ms/batched_material.hpp>

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

std::shared_ptr<BatchedMaterial> Material::Builder::buildBatched(Assets& assets) {
    if (_batch_material && _batch_count != 0) {
        makeBatched(*_batch_material, _batch_count);
    }

    if (!_batch_materials.empty()) {
        makeBatched(*_batch_materials[0], _batch_count != 0 ? _batch_count : _batch_materials.size());
    }

    auto material = std::shared_ptr<BatchedMaterial>(new BatchedMaterial(*this));

    for (size_t i = 0; i < _batch_materials.size(); ++i) {
        material->set(i, *_batch_materials[i], true);
    }

    assets.materials.add(material->name, material);

    return material;
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

Material::Builder& Material::Builder::batch_count(size_t size) {
    _batch_count = size;
    return *this;
}

void Material::Builder::makeBatched(const Material& material, size_t batch_count) {
    const std::string batch_postfix = "_array";

    // map properties to batched arrays
    for (const auto& [property, uniform]: material.properties) {
        const auto name = uniform->getName() + batch_postfix;

        switch (property) {
            case Property::Diffuse:
            case Property::Normal:
            case Property::EmissiveMask:
            case Property::BlendMask:
            case Property::MetallicTexture:
            case Property::RoughnessTexture:
            case Property::AmbientOcclusionTexture:
            case Property::ORM:
                uniforms[name] = std::make_unique<UniformSampler>(
                    name,
                    Texture::Builder::asArray(*static_cast<UniformSampler&>(*uniform).getSampler(), batch_count)
                );
                break;

            case Property::Color:
            case Property::EmissiveColor:
                uniforms[name] = std::make_unique<UniformValueArray<glm::vec4>>(name,batch_count);
                break;

            case Property::Metallic:
            case Property::Roughness:
            case Property::IoR:
            case Property::Absorption:
            case Property::MicroThickness:
            case Property::Thickness:
            case Property::Reflectance:
            case Property::Transmission:
                uniforms[name] = std::make_unique<UniformValueArray<float>>(name,batch_count);
                break;
        }
    }

    // map uniforms to batched arrays
    for (const auto& [_, uniform]: material.uniforms) {
        const auto name = uniform->getName() + batch_postfix;

        switch (uniform->getType()) {
            case UniformType::Value:
                switch (uniform->getValueType()) {
                    case DataType::Float:
                        uniforms[name] = std::make_unique<UniformValueArray<float>>(name,batch_count);
                        break;
                    case DataType::Int:
                        uniforms[name] = std::make_unique<UniformValueArray<int32_t>>(name,batch_count);
                        break;
                    case DataType::Uint:
                        uniforms[name] = std::make_unique<UniformValueArray<uint32_t>>(name,batch_count);
                        break;
                    case DataType::Vec2:
                        uniforms[name] = std::make_unique<UniformValueArray<glm::vec2>>(name,batch_count);
                        break;
                    case DataType::Vec3:
                        uniforms[name] = std::make_unique<UniformValueArray<glm::vec3>>(name,batch_count);
                        break;
                    case DataType::Vec4:
                        uniforms[name] = std::make_unique<UniformValueArray<glm::vec4>>(name,batch_count);
                        break;
                    case DataType::Mat3:
                        uniforms[name] = std::make_unique<UniformValueArray<glm::mat3>>(name,batch_count);
                        break;
                    case DataType::Mat4:
                        uniforms[name] = std::make_unique<UniformValueArray<glm::mat4>>(name,batch_count);
                        break;
                    case DataType::IVec4:
                        uniforms[name] = std::make_unique<UniformValueArray<glm::ivec4>>(name,batch_count);
                        break;
                }
                break;

            case UniformType::Sampler:
                uniforms[name] = std::make_unique<UniformSampler>(
                    name,
                    Texture::Builder::asArray(*static_cast<UniformSampler&>(*uniform).getSampler(), batch_count)
                );
                break;
            case UniformType::Time:
                uniforms[uniform->getName()] = uniform->clone();
                break;

            case UniformType::ValueArray:
                throw material_exception{"batching of uniform arrays is not supported yet!"};
        }
    }

    for (const auto& [property, uniform] : material.properties) {
        properties[property] = uniform->clone();
    }

    _blending = material.blending;
    _shading = material.shading;
    _two_sided = material.two_sided;
    _refraction = material.refraction;
    _name = material.name + "_batched";
    _model_shaders = material.model_shaders;
    vertex_snippet = material.vertex_snippet;
    fragment_snippet = material.fragment_snippet;
    global_snippet = material.global_snippet;
    shading_snippet = material.shading_snippet;
}

Material::Builder &Material::Builder::batch(const std::shared_ptr<Material>& material) {
    _batch_material = material;
    return *this;
}

Material::Builder &Material::Builder::add_batch(const std::shared_ptr<Material>& material) {
    _batch_materials.emplace_back(material);
    return *this;
}
