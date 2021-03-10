#pragma once

#include <memory>

namespace LimitlessEngine {
    class Material;
    class ByteBuffer;
    class MaterialBuilder;
    class CustomMaterialBuilder;

    class MaterialSerializer {
    private:
        void deserializeProperties(ByteBuffer& buffer, MaterialBuilder& builder);
        void deserializeCustomUniforms(ByteBuffer& buffer, CustomMaterialBuilder& builder);
    public:
        ByteBuffer serialize(const Material& material);
        std::shared_ptr<Material> deserialize(ByteBuffer& buffer);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const Material& material);
    ByteBuffer& operator>>(ByteBuffer& buffer, std::shared_ptr<Material>& material);
}
