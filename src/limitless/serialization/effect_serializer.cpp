#include <limitless/serialization/effect_serializer.hpp>

#include <limitless/serialization/emitter_serializer.hpp>
#include <limitless/instances/effect_instance.hpp>

using namespace Limitless;
using namespace Limitless::fx;

ByteBuffer EffectSerializer::serialize(const EffectInstance& instance) {
    ByteBuffer buffer;

    buffer << VERSION;

    buffer << instance.name
           << instance.emitters;

    return buffer;
}

std::shared_ptr<EffectInstance> EffectSerializer::deserialize(Assets& assets, ByteBuffer& buffer) {
    uint8_t version {};

    buffer >> version;

    if (version != VERSION) {
        throw std::runtime_error("Wrong effect serializer version! " + std::to_string(VERSION) + " vs " + std::to_string(version));
    }

    fx::EffectBuilder builder {assets};
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

ByteBuffer& Limitless::operator<<(ByteBuffer& buffer, const EffectInstance& effect) {
    EffectSerializer serializer;
    buffer << serializer.serialize(effect);
    return buffer;
}

ByteBuffer& Limitless::operator>>(ByteBuffer& buffer, const AssetDeserializer<std::shared_ptr<EffectInstance>>& asset) {
    EffectSerializer serializer;
    auto& [assets, effect] = asset;
    effect = serializer.deserialize(assets, buffer);
    return buffer;
}
