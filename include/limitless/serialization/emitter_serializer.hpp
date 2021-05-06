#pragma once

#include <memory>
#include <limitless/fx/effect_builder.hpp>

namespace Limitless {
    class ByteBuffer;
    class Assets;
    class Context;
}

namespace Limitless::fx {
    class AbstractEmitter;
    class EmitterSpawn;
    class EffectBuilder;

    class EmitterSerializer {
    public:
        ByteBuffer serialize(const AbstractEmitter& emitter);
        void deserialize(Context& context, Assets& ctx, ByteBuffer& buffer, EffectBuilder& builder);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const EmitterSpawn& spawn);
    ByteBuffer& operator>>(ByteBuffer& buffer, EmitterSpawn& pair);

    ByteBuffer& operator<<(ByteBuffer& buffer, const AbstractEmitter& emitter);
}