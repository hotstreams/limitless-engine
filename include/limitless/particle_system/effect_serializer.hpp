#pragma once

#include <memory>

namespace LimitlessEngine {
    class EffectInstance;
    class ByteBuffer;

    class EffectSerializer {
    public:
        ByteBuffer serialize(const EffectInstance& instance);
        std::shared_ptr<EffectInstance> deserialize(ByteBuffer& buffer);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const EffectInstance& effect);
    ByteBuffer& operator>>(ByteBuffer& buffer, std::shared_ptr<EffectInstance>& instance);
}