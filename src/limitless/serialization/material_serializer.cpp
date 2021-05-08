#include <limitless/serialization/material_serializer.hpp>

#include <limitless/ms/material_builder.hpp>
#include <limitless/serialization/uniform_serializer.hpp>
#include <limitless/serialization/asset_deserializer.hpp>
#include <limitless/util/bytebuffer.hpp>
#include <iostream>

using namespace Limitless::ms;
using namespace Limitless;

void MaterialSerializer::deserialize(ByteBuffer& buffer, Context& context, Assets& assets, const RenderSettings& settings, MaterialBuilder& builder) {
    std::map<Property, std::unique_ptr<Uniform>> properties;
    std::unordered_map<std::string, std::unique_ptr<Uniform>> uniforms;
    Blending blending{};
    Shading shading{};
    std::string name;
    std::string vertex_code;
    std::string fragment_code;
    std::string global_code;

    buffer >> name
           >> shading
           >> blending
           >> AssetDeserializer<decltype(properties)>{context, assets, settings, properties}
           >> AssetDeserializer<decltype(uniforms)>{context, assets, settings, uniforms}
           >> vertex_code
           >> fragment_code
           >> global_code;

    builder .setName(name)
            .setShading(shading)
            .setBlending(blending)
            .set(std::move(properties))
            .set(std::move(uniforms))
            .setVertexSnippet(vertex_code)
            .setFragmentSnippet(fragment_code)
            .setGlobalSnippet(global_code);
}

ByteBuffer MaterialSerializer::serialize(const Material& material) {
    ByteBuffer buffer;

    buffer << material.getName()
           << material.getShading()
           << material.getBlending()
           << material.getProperties()
           << material.getUniforms()
           << material.getVertexSnippet()
           << material.getFragmentSnippet()
           << material.getGlobalSnippet()
           << material.getModelShaders();

    return buffer;
}

std::shared_ptr<Material> MaterialSerializer::deserialize(Context& ctx, Assets& assets, const RenderSettings& settings, ByteBuffer& buffer) {
    MaterialBuilder builder {ctx, assets, settings};

    deserialize(buffer, ctx, assets, settings, builder);

    ModelShaders compile_models;
    buffer >> compile_models;

    builder.setModelShaders(compile_models);

    return builder.build();
}

ByteBuffer& Limitless::ms::operator<<(ByteBuffer& buffer, const Material& material) {
    MaterialSerializer serializer;
    buffer << serializer.serialize(material);
    return buffer;
}

ByteBuffer& Limitless::ms::operator>>(ByteBuffer& buffer, const AssetDeserializer<std::shared_ptr<Material>>& asset) {
    MaterialSerializer serializer;
    auto& [context, assets, settings, material] = asset;
    material = serializer.deserialize(context, assets, settings, buffer);
    return buffer;
}
