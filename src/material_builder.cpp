#include <material_builder.hpp>
#include <shader_compiler.hpp>
#include <assets.hpp>

using namespace GraphicsEngine;

MaterialBuilder& MaterialBuilder::add(PropertyType type, float value) {
    switch (type) {
        case PropertyType::Shininess:
            properties.emplace(type, new UniformValue<float>("material_shininess", value));
            break;
        case PropertyType::Metallic:
            properties.emplace(type, new UniformValue<float>("material_metallic", value));
            break;
        case PropertyType::Roughness:
            properties.emplace(type, new UniformValue<float>("material_roughness", value));
            break;
        case PropertyType::Color:
        case PropertyType::EmissiveColor:
        case PropertyType::Normal:
        case PropertyType::Displacement:
        case PropertyType::Diffuse:
        case PropertyType::Specular:
        case PropertyType::EmissiveMask:
        case PropertyType::BlendMask:
            throw std::runtime_error("Wrong data for material property.");
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::add(PropertyType type, const glm::vec4& value) {
    switch (type) {
        case PropertyType::Color:
            properties.emplace(type, new UniformValue<glm::vec4>("material_color", value));
            break;
        case PropertyType::EmissiveColor:
            properties.emplace(type, new UniformValue<glm::vec4>("material_emissive_color", value));
            break;
        case PropertyType::Shininess:
        case PropertyType::Normal:
        case PropertyType::Displacement:
        case PropertyType::Metallic:
        case PropertyType::Roughness:
        case PropertyType::Diffuse:
        case PropertyType::Specular:
        case PropertyType::EmissiveMask:
        case PropertyType::BlendMask:
            throw std::runtime_error("Wrong data for material property.");
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::add(PropertyType type, std::shared_ptr<Texture> texture) {
    switch (type) {
        case PropertyType::Color:
        case PropertyType::Shininess:
        case PropertyType::Metallic:
        case PropertyType::Roughness:
        case PropertyType::EmissiveColor:
            throw std::runtime_error("Wrong data for material property.");
        case PropertyType::Diffuse:
            properties.emplace(type, new UniformSampler("material_diffuse", std::move(texture)));
            break;
        case PropertyType::Specular:
            properties.emplace(type, new UniformSampler("material_specular", std::move(texture)));
            break;
        case PropertyType::Normal:;
            properties.emplace(type, new UniformSampler("material_normal", std::move(texture)));
            break;
        case PropertyType::Displacement:
            properties.emplace(type, new UniformSampler("material_displacement", std::move(texture)));
            break;
        case PropertyType::EmissiveMask:
            properties.emplace(type, new UniformSampler("material_emissive_mask", std::move(texture)));
            break;
        case PropertyType::BlendMask:
            properties.emplace(type, new UniformSampler("material_blend_mask", std::move(texture)));
            break;
    }
    return *this;
}

size_t MaterialBuilder::getMaterialBufferSize() const noexcept {
    size_t size = 0;
    for (const auto& [type, prop] : properties) {
        switch (type) {
            case PropertyType::Color:
            case PropertyType::EmissiveColor:
                size += sizeof(glm::vec4);
                break;
            case PropertyType::Shininess:
            case PropertyType::Metallic:
            case PropertyType::Roughness:
                size += sizeof(float);
                break;
            case PropertyType::Diffuse:
            case PropertyType::Specular:
            case PropertyType::Normal:
            case PropertyType::Displacement:
            case PropertyType::EmissiveMask:
            case PropertyType::BlendMask:
                size += sizeof(uint64_t);
                break;
        }
    }
    return size;
}

std::shared_ptr<Material> MaterialBuilder::build(const std::string& name, const RequiredShaders& shaders) {
    uint64_t material_index = 0;
    auto material_type = getMaterialType();
    auto material_buffer_size = getMaterialBufferSize();

    // sets current unique-material-type index
    auto found = unique_materials.find(material_type);
    material_index = (found != unique_materials.end()) ? found->second : shader_index++;

    auto material = std::shared_ptr<Material>(new Material(std::move(properties), blending, shading, name, material_index));
    // initializes material buffer
    std::vector<std::byte> data(material_buffer_size);
    material->material_buffer = BufferBuilder::buildIndexed("material_buffer", Buffer::Type::Uniform, data, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);

    // in case that's a new one
    if (found == unique_materials.end()) {
        // compile whatever we need for that material
        ShaderCompiler::compile(material_type, material_index, shaders);
        unique_materials.emplace(material_type, material_index);
    }

    // clear for new material state
    properties.clear();
    blending = Blending::Opaque;
    shading = Shading::Unlit;

    assets.materials.add(name, material);
    return material;
}

MaterialType MaterialBuilder::getMaterialType() const noexcept {
    std::vector<PropertyType> props;
    for (const auto& [type, uniform] : properties) {
        props.emplace_back(type);
    }

    return MaterialType{props, blending, shading};
}

MaterialBuilder& MaterialBuilder::setBlending(Blending _blending) noexcept {
    blending = _blending;
    return *this;
}

MaterialBuilder &MaterialBuilder::setShading(Shading _shading) noexcept {
    shading = _shading;
    return *this;
}
