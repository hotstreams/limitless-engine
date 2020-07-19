#include <material.hpp>
#include <bindless_texture.hpp>

using namespace GraphicsEngine;

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
    size_t offset = 0;
    for (const auto& [type, prop] : properties) {
        switch (type) {
            case PropertyType::Color:
            case PropertyType::EmissiveColor:
                std::memcpy(data.data(), &static_cast<UniformValue<glm::vec4>&>(*properties.at(type)).getValue(), sizeof(glm::vec4));
                offset += sizeof(glm::vec4);
                break;
            case PropertyType::Shininess:
            case PropertyType::Metallic:
            case PropertyType::Roughness:
                std::memcpy(data.data() + offset, &static_cast<UniformValue<float>&>(*properties.at(type)).getValue(), sizeof(float));
                offset += sizeof(float);
                break;
            case PropertyType::Diffuse:
            case PropertyType::Specular:
            case PropertyType::Normal:
            case PropertyType::Displacement:
            case PropertyType::EmissiveMask:
            case PropertyType::BlendMask:
                if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                    auto texture = static_cast<UniformSampler&>(*properties.at(type)).getSampler();
                    std::memcpy(data.data() + offset, &static_cast<BindlessTexture&>(*texture).getHandle(), sizeof(uint64_t));
                    offset += sizeof(uint64_t);
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
    return static_cast<UniformValue<glm::vec4>&>(*properties.at(PropertyType::Color));
}

UniformValue<glm::vec4>& Material::getEmissiveColor() const {
    return static_cast<UniformValue<glm::vec4>&>(*properties.at(PropertyType::EmissiveColor));
}

UniformValue<float>& Material::getShininess() const {
    return static_cast<UniformValue<float>&>(*properties.at(PropertyType::Shininess));
}

UniformValue<float>& Material::getMetallic() const {
    return static_cast<UniformValue<float>&>(*properties.at(PropertyType::Metallic));
}

UniformValue<float>& Material::getRoughness() const {
    return static_cast<UniformValue<float>&>(*properties.at(PropertyType::Roughness));
}

UniformSampler& Material::getDiffuse() const {
    return static_cast<UniformSampler&>(*properties.at(PropertyType::Diffuse));
}

UniformSampler& Material::getSpecular() const {
    return static_cast<UniformSampler&>(*properties.at(PropertyType::Specular));
}

UniformSampler& Material::getNormal() const {
    return static_cast<UniformSampler&>(*properties.at(PropertyType::Normal));
}

UniformSampler& Material::getDisplacement() const {
    return static_cast<UniformSampler&>(*properties.at(PropertyType::Displacement));
}

UniformSampler& Material::getEmissiveMask() const {
    return static_cast<UniformSampler&>(*properties.at(PropertyType::EmissiveMask));
}

UniformSampler& Material::getBlendMask() const {
    return static_cast<UniformSampler&>(*properties.at(PropertyType::BlendMask));
}

Material::Material(decltype(properties) &&properties, Blending blending, Shading shading, std::string name, uint64_t shader_index) noexcept
    : properties{std::move(properties)}, blending{blending}, shading{shading}, name{std::move(name)}, shader_index{shader_index} {

}

bool GraphicsEngine::operator<(const MaterialType& lhs, const MaterialType& rhs) noexcept {
    return std::tie(lhs.properties, lhs.shading, lhs.blending) < std::tie(rhs.properties, rhs.shading, rhs.blending);
}
