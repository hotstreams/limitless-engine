#pragma once

#include <memory>
#include <limitless/serialization/asset_deserializer.hpp>

namespace LimitlessEngine {
    class Context;
    class Assets;
    class Material;
    class ByteBuffer;
    class MaterialBuilder;
    class CustomMaterialBuilder;

    class MaterialSerializer {
    private:
        void deserializeProperties(ByteBuffer& buffer, Context& context, Assets& assets, MaterialBuilder& builder);
        void deserializeCustomUniforms(ByteBuffer& buffer, Context& context, Assets& assets, CustomMaterialBuilder& builder);
    public:
        ByteBuffer serialize(const Material& material);
        std::shared_ptr<Material> deserialize(Context& ctx, Assets& assets, ByteBuffer& buffer);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const Material& material);
    ByteBuffer& operator>>(ByteBuffer& buffer, const AssetDeserializer<std::shared_ptr<Material>>& material);
}
