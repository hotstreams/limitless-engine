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

std::shared_ptr<EffectInstance> EffectSerializer::deserialize(Context& context, Assets& assets, ByteBuffer& buffer) {
    EffectBuilder builder {context, assets};
    std::string name;
    size_t size;

    buffer >> name >> size;

    builder.create(name);

    for (size_t i = 0; i < size; ++i) {
        EmitterSerializer serializer;
        serializer.deserialize(context, assets, buffer, builder);
    }

    return builder.build();
}

ByteBuffer& LimitlessEngine::operator<<(ByteBuffer& buffer, const EffectInstance& effect) {
    EffectSerializer serializer;
    buffer << serializer.serialize(effect);
    return buffer;
}

ByteBuffer& LimitlessEngine::operator>>(ByteBuffer& buffer, const AssetDeserializer<std::shared_ptr<EffectInstance>>& asset) {
    EffectSerializer serializer;
    auto& [context, assets, effect] = asset;
    effect = serializer.deserialize(context, assets, buffer);
    return buffer;
}
