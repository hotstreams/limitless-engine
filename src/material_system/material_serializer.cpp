#include <limitless/material_system/material_serializer.hpp>

#include <limitless/material_system/custom_material_builder.hpp>
#include <limitless/core/uniform_serializer.hpp>
#include <limitless/util/bytebuffer.hpp>

using namespace LimitlessEngine;

void MaterialSerializer::deserializeProperties(ByteBuffer& buffer, MaterialBuilder& builder) {
    decltype(Material::properties) properties;
    Blending blending{};
    Shading shading{};
    std::string name;

    buffer >> name
           >> shading
           >> blending
           >> properties;

    builder .create(name)
            .setShading(shading)
            .setBlending(blending)
            .setProperties(std::move(properties));
}

void MaterialSerializer::deserializeCustomUniforms(ByteBuffer& buffer, CustomMaterialBuilder& builder) {
    decltype(CustomMaterial::uniforms) uniforms;
    std::string vertex_code;
    std::string fragment_code;

    buffer >> uniforms
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

    return buffer;
}

std::shared_ptr<Material> MaterialSerializer::deserialize(ByteBuffer& buffer) {
    bool isCustom{};
    buffer >> isCustom;

    if (isCustom) {
        CustomMaterialBuilder builder;

        deserializeProperties(buffer, builder);
        deserializeCustomUniforms(buffer, builder);

        return builder.build();
    } else {
        MaterialBuilder builder;

        deserializeProperties(buffer, builder);

        return builder.build();
    }
}

ByteBuffer& LimitlessEngine::operator<<(ByteBuffer& buffer, const Material& material) {
    MaterialSerializer serializer;
    buffer << serializer.serialize(material);
    return buffer;
}

ByteBuffer& LimitlessEngine::operator>>(ByteBuffer& buffer, std::shared_ptr<Material>& material) {
    MaterialSerializer serializer;
    material = serializer.deserialize(buffer);
    return buffer;
}
