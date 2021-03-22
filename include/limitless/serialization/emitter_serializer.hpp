#pragma once

#include <memory>

namespace LimitlessEngine {
    class Emitter;
    struct EmitterSpawn;
    class ByteBuffer;
    class EffectBuilder;
    class Assets;

    class EmitterSerializer {
    public:
        ByteBuffer serialize(const Emitter& emitter);
        void deserialize(Assets& ctx, ByteBuffer& buffer, EffectBuilder& builder);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const EmitterSpawn& spawn);
    ByteBuffer& operator>>(ByteBuffer& buffer, EmitterSpawn& pair);

    ByteBuffer& operator<<(ByteBuffer& buffer, const Emitter& emitter);
}