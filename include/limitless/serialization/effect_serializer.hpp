#pragma once

#include <limitless/serialization/asset_deserializer.hpp>
#include <memory>

namespace Limitless {
    class EffectInstance;
    class ByteBuffer;
    class Assets;
    class Context;
}

namespace Limitless {
    class EffectSerializer {
    private:
        static constexpr uint8_t VERSION = 0x1;
    public:
        ByteBuffer serialize(const EffectInstance& instance);
        std::shared_ptr<EffectInstance> deserialize(Assets& assets, ByteBuffer& buffer);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const EffectInstance& effect);
    ByteBuffer& operator>>(ByteBuffer& buffer, const AssetDeserializer<std::shared_ptr<EffectInstance>>& asset);
}