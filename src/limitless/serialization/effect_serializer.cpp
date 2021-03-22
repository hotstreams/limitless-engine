#include <limitless/serialization/effect_serializer.hpp>

#include <limitless/util/bytebuffer.hpp>
#include <limitless/particle_system/effect_builder.hpp>
#include <limitless/serialization/emitter_serializer.hpp>

using namespace LimitlessEngine;

ByteBuffer EffectSerializer::serialize(const EffectInstance& instance) {
    ByteBuffer buffer;

    buffer << instance.name
           << instance.emitters;

    return buffer;
}

std::shared_ptr<EffectInstance> EffectSerializer::deserialize(Assets& assets, ByteBuffer& buffer) {
    EffectBuilder builder {assets};
    std::string name;
    size_t size;

    buffer >> name >> size;

    builder.create(name);

    for (size_t i = 0; i < size; ++i) {
        EmitterSerializer serializer;
        serializer.deserialize(assets, buffer, builder);
    }

    return builder.build();
}

ByteBuffer& LimitlessEngine::operator<<(ByteBuffer& buffer, const EffectInstance& effect) {
    EffectSerializer serializer;
    buffer << serializer.serialize(effect);
    return buffer;
}

ByteBuffer& LimitlessEngine::operator>>(ByteBuffer& buffer, const std::pair<Assets&, std::shared_ptr<EffectInstance>&>& pair) {
    EffectSerializer serializer;
    auto& [assets, effect] = pair;
    effect = serializer.deserialize(assets, buffer);
    return buffer;
}
