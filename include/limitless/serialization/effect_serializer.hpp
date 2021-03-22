#pragma once

#include <memory>

namespace LimitlessEngine {
    class EffectInstance;
    class ByteBuffer;
    class Assets;

    class EffectSerializer {
    public:
        ByteBuffer serialize(const EffectInstance& instance);
        std::shared_ptr<EffectInstance> deserialize(Assets& assets, ByteBuffer& buffer);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const EffectInstance& effect);
    ByteBuffer& operator>>(ByteBuffer& buffer, const std::pair<Assets&, std::shared_ptr<EffectInstance>&>& pair);
}