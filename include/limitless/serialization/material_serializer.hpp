#pragma once

#include <memory>
#include <limitless/serialization/asset_deserializer.hpp>
#include <limitless/ms/material.hpp>
#include <cstdint>

namespace Limitless {
    class Context;
    class Assets;
    class ByteBuffer;
}

namespace Limitless {
    class MaterialSerializer {
    private:
        static constexpr uint8_t VERSION = 0x2;

        void deserialize(ByteBuffer& buffer, Assets& assets, ms::Material::Builder& builder);
    public:
        ByteBuffer serialize(const ms::Material& material);
        std::shared_ptr<ms::Material> deserialize(Assets& assets, ByteBuffer& buffer);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const ms::Material& material);
    ByteBuffer& operator>>(ByteBuffer& buffer, const AssetDeserializer<std::shared_ptr<ms::Material>>& material);
}
