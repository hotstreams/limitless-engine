#pragma once

#include <memory>
#include <limitless/fx/effect_builder.hpp>

namespace Limitless {
    class ByteBuffer;
    class Assets;
    class Context;
    class RenderSettings;

    namespace fx {
        class AbstractEmitter;
        class EmitterSpawn;
        class EffectBuilder;
    }
}

namespace Limitless {
    class EmitterSerializer {
    private:
        static constexpr uint8_t VERSION = 0x1;
    public:
        ByteBuffer serialize(const fx::AbstractEmitter& emitter);
        void deserialize(Assets& ctx, ByteBuffer& buffer, fx::EffectBuilder& builder);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const fx::EmitterSpawn& spawn);
    ByteBuffer& operator>>(ByteBuffer& buffer, fx::EmitterSpawn& pair);

    ByteBuffer& operator<<(ByteBuffer& buffer, const fx::AbstractEmitter& emitter);
}