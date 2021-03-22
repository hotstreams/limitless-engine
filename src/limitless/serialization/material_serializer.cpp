#include <limitless/serialization/material_serializer.hpp>

#include <limitless/material_system/custom_material_builder.hpp>
#include <limitless/serialization/uniform_serializer.hpp>
#include <limitless/serialization/asset_deserializer.hpp>
#include <limitless/util/bytebuffer.hpp>
#include <iostream>

using namespace LimitlessEngine;

void MaterialSerializer::deserializeProperties(ByteBuffer& buffer, Assets& assets, MaterialBuilder& builder) {
    decltype(Material::properties) properties;
    Blending blending{};
    Shading shading{};
    std::string name;

    buffer >> name
           >> shading
           >> blending
           >> AssetDeserializer<decltype(properties)>{assets, properties};

    builder .create(name)
            .setShading(shading)
            .setBlending(blending)
            .setProperties(std::move(properties));
}

void MaterialSerializer::deserializeCustomUniforms(ByteBuffer& buffer, Assets& assets, CustomMaterialBuilder& builder) {
    decltype(CustomMaterial::uniforms) uniforms;
    std::string vertex_code;
    std::string fragment_code;

    buffer >> AssetDeserializer<decltype(uniforms)>{assets, uniforms}
           >> vertex_code
           >> fragment_code;

    builder .setUniforms(std::move(uniforms))
            .setVertexCode(vertex_code)
            .setFragmentCode(fragment_code);
}

ByteBuffer MaterialSerializer::serialize(const Material& material) {
    ByteBuffer buffer;

    buffer << material.isCustom()
           << material.name
           << material.shading
           << material.blending
           << material.properties;

    if (material.isCustom()) {
        const auto& custom_material = static_cast<const CustomMaterial&>(material);
        buffer << custom_material.uniforms
               << custom_material.vertex_code
               << custom_material.fragment_code;
    }

    buffer << material.model_shaders;

    return buffer;
}

std::shared_ptr<Material> MaterialSerializer::deserialize(Assets& assets, ByteBuffer& buffer) {
    bool isCustom{};
    buffer >> isCustom;

    if (isCustom) {
        CustomMaterialBuilder builder {assets};

        deserializeProperties(buffer, assets, builder);
        deserializeCustomUniforms(buffer, assets, builder);

        ModelShaders compile_models;
        buffer >> compile_models;

        return builder.build(compile_models);
    } else {
        MaterialBuilder builder {assets};

        deserializeProperties(buffer, assets, builder);

        ModelShaders compile_models;
        buffer >> compile_models;

        return builder.build(compile_models);
    }
}

ByteBuffer& LimitlessEngine::operator<<(ByteBuffer& buffer, const Material& material) {
    MaterialSerializer serializer;
    buffer << serializer.serialize(material);
    return buffer;
}

ByteBuffer& LimitlessEngine::operator>>(ByteBuffer& buffer, const AssetDeserializer<std::shared_ptr<Material>>& asset) {
    MaterialSerializer serializer;
    auto& [assets, material] = asset;
    material = serializer.deserialize(assets, buffer);
    return buffer;
}
