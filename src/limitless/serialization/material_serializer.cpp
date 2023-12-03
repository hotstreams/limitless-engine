#include <limitless/serialization/material_serializer.hpp>

#include <limitless/ms/material_builder.hpp>
#include <limitless/serialization/uniform_serializer.hpp>
#include <limitless/serialization/asset_deserializer.hpp>
#include <limitless/util/bytebuffer.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/uniform/uniform_value.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>

using namespace Limitless::ms;
using namespace Limitless;

void MaterialSerializer::deserialize(ByteBuffer& buffer, Assets& assets, Material::Builder& builder) {
    std::map<Property, std::unique_ptr<Uniform>> properties;
    std::map<std::string, std::unique_ptr<Uniform>> uniforms;
    Blending blending{};
    Shading shading{};
    bool two_sided {};
    std::string name;
    std::string vertex_code;
    std::string fragment_code;
    std::string global_code;
    std::string tes_code;

    buffer >> name
           >> shading
           >> blending
           >> two_sided
           >> AssetDeserializer<decltype(properties)>{assets, properties}
           >> AssetDeserializer<decltype(uniforms)>{assets, uniforms}
           >> vertex_code
           >> fragment_code
           >> global_code
           >> tes_code;

    builder .name(name)
            .shading(shading)
            .blending(blending)
            .two_sided(two_sided)
            .vertex(vertex_code)
            .fragment(fragment_code)
            .global(global_code);

    for (auto& [property, uniform]: properties) {
        switch (property) {
            case Property::Color:
                builder.color(static_cast<UniformValue<glm::vec4>&>(*uniform).getValue());
                break;
            case Property::EmissiveColor:
                builder.emissive_color(static_cast<UniformValue<glm::vec3>&>(*uniform).getValue());
                break;
            case Property::Diffuse:
                builder.diffuse(static_cast<UniformSampler&>(*uniform).getSampler());
                break;
            case Property::Normal:
                builder.normal(static_cast<UniformSampler&>(*uniform).getSampler());
                break;
            case Property::EmissiveMask:
                builder.emissive_mask(static_cast<UniformSampler&>(*uniform).getSampler());
                break;
            case Property::BlendMask:
                builder.blend_mask(static_cast<UniformSampler&>(*uniform).getSampler());
                break;
            case Property::MetallicTexture:
                builder.metallic(static_cast<UniformSampler&>(*uniform).getSampler());
                break;
            case Property::RoughnessTexture:
                builder.roughness(static_cast<UniformSampler&>(*uniform).getSampler());
                break;
            case Property::AmbientOcclusionTexture:
                builder.ao(static_cast<UniformSampler&>(*uniform).getSampler());
                break;
            case Property::ORM:
                builder.orm(static_cast<UniformSampler&>(*uniform).getSampler());
                break;
            case Property::Metallic:
                builder.metallic(static_cast<UniformValue<float>&>(*uniform).getValue());
                break;
            case Property::Roughness:
                builder.roughness(static_cast<UniformValue<float>&>(*uniform).getValue());
                break;
            case Property::IoR:
                builder.ior(static_cast<UniformValue<float>&>(*uniform).getValue());
                break;
            case Property::Absorption:
                builder.absorption(static_cast<UniformValue<float>&>(*uniform).getValue());
                break;
            case Property::MicroThickness:
                builder.microthickness(static_cast<UniformValue<float>&>(*uniform).getValue());
                break;
            case Property::Thickness:
                builder.thickness(static_cast<UniformValue<float>&>(*uniform).getValue());
                break;
            case Property::Reflectance:
                builder.reflectance(static_cast<UniformValue<float>&>(*uniform).getValue());
                break;
            case Property::Transmission:
                builder.transmission(static_cast<UniformValue<float>&>(*uniform).getValue());
                break;
        }
    }

    for (auto& [name, uniform]: uniforms) {
        switch (uniform->getType()) {
            case UniformType::Value:
                switch (uniform->getValueType()) {
                    case UniformValueType::Float:
                        builder.custom(name, static_cast<UniformValue<float>&>(*uniform).getValue());
                        break;
                    case UniformValueType::Int:
                        builder.custom(name, static_cast<UniformValue<int32_t>&>(*uniform).getValue());
                        break;
                    case UniformValueType::Uint:
                        builder.custom(name, static_cast<UniformValue<uint32_t>&>(*uniform).getValue());
                        break;
                    case UniformValueType::Vec2:
                        builder.custom(name, static_cast<UniformValue<glm::vec2>&>(*uniform).getValue());
                        break;
                    case UniformValueType::Vec3:
                        builder.custom(name, static_cast<UniformValue<glm::vec3>&>(*uniform).getValue());
                        break;
                    case UniformValueType::Vec4:
                        builder.custom(name, static_cast<UniformValue<glm::vec4>&>(*uniform).getValue());
                        break;
                    case UniformValueType::Mat3:
                        builder.custom(name, static_cast<UniformValue<glm::mat3>&>(*uniform).getValue());
                        break;
                    case UniformValueType::Mat4:
                        builder.custom(name, static_cast<UniformValue<glm::mat4>&>(*uniform).getValue());
                        break;
                }
                break;
            case UniformType::Sampler:
                builder.custom(name, static_cast<UniformSampler&>(*uniform).getSampler());
                break;
            case UniformType::Time:
                builder.time();
                break;
        }
    }
}

ByteBuffer MaterialSerializer::serialize(const Material& material) {
    ByteBuffer buffer;

    buffer << VERSION;

    buffer << material.getName()
           << material.getShading()
           << material.getBlending()
           << material.getTwoSided()
           << material.getProperties()
           << material.getUniforms()
           << material.getVertexSnippet()
           << material.getFragmentSnippet()
           << material.getGlobalSnippet()
           << material.getModelShaders();

    return buffer;
}

std::shared_ptr<Material> MaterialSerializer::deserialize(Assets& assets, ByteBuffer& buffer) {
    uint8_t version {};

    buffer >> version;

    if (version != VERSION) {
        throw std::runtime_error("Wrong material serializer version! " + std::to_string(VERSION) + " vs " + std::to_string(version));
    }

    Material::Builder builder;

    deserialize(buffer, assets, builder);

    InstanceTypes compile_models;
    buffer >> compile_models;

    builder.models(compile_models);

//    try {
        return builder.build(assets);
//    } catch (const resource_container_error& ex) {
//        return assets.materials.at(builder.getName());
//    }
}

ByteBuffer& Limitless::operator<<(ByteBuffer& buffer, const Material& material) {
    MaterialSerializer serializer;
    buffer << serializer.serialize(material);
    return buffer;
}

ByteBuffer& Limitless::operator>>(ByteBuffer& buffer, const AssetDeserializer<std::shared_ptr<Material>>& asset) {
    MaterialSerializer serializer;
    auto& [assets, material] = asset;
    material = serializer.deserialize(assets, buffer);
    return buffer;
}
