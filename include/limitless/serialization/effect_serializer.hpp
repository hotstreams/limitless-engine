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
    public:
        ByteBuffer serialize(const EffectInstance& instance);
        std::shared_ptr<EffectInstance> deserialize(Context& context, Assets& assets, ByteBuffer& buffer);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const EffectInstance& effect);
    ByteBuffer& operator>>(ByteBuffer& buffer, const AssetDeserializer<std::shared_ptr<EffectInstance>>& asset);
}