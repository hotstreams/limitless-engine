#pragma once

#include <memory>
#include <limitless/particle_system/effect_builder.hpp>


namespace LimitlessEngine {
    class Emitter;
    struct EmitterSpawn;
    class ByteBuffer;
    class EffectBuilder;
    class Assets;
    class Context;

    class EmitterSerializer {
    public:
        ByteBuffer serialize(const Emitter& emitter);
        void deserialize(Context& context, Assets& ctx, ByteBuffer& buffer, EffectBuilder& builder);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const EmitterSpawn& spawn);
    ByteBuffer& operator>>(ByteBuffer& buffer, EmitterSpawn& pair);

    ByteBuffer& operator<<(ByteBuffer& buffer, const Emitter& emitter);
}