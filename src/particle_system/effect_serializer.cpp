#include <particle_system/effect_serializer.hpp>

#include <util/bytebuffer.hpp>
#include <particle_system/effect_builder.hpp>
#include <particle_system/emitter_serializer.hpp>

using namespace LimitlessEngine;

ByteBuffer EffectSerializer::serialize(std::string name, const EffectInstance& instance) {
    ByteBuffer buffer;

    buffer << name
           << instance.emitters;

    return buffer;
}

std::shared_ptr<EffectInstance> EffectSerializer::deserialize(ByteBuffer& buffer) {
    EffectBuilder builder;
    std::string name;
    size_t size;

    buffer >> name >> size;

    builder.create(name);

    for (size_t i = 0; i < size; ++i) {
        EmitterSerializer serializer;
        serializer.deserialize(buffer, builder);
    }

    return builder.build();
}
