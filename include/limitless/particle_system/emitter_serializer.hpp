#pragma once

#include <memory>

namespace LimitlessEngine {
    class Emitter;
    class EmitterSpawn;
    class ByteBuffer;
    class EffectBuilder;

    class EmitterSerializer {
    public:
        ByteBuffer serialize(const Emitter& emitter);
        void deserialize(ByteBuffer& buffer, EffectBuilder& builder);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const EmitterSpawn& spawn);
    ByteBuffer& operator>>(ByteBuffer& buffer, EmitterSpawn& spawn);

    ByteBuffer& operator<<(ByteBuffer& buffer, const Emitter& emitter);
}