#include <limitless/serialization/effect_serializer.hpp>

#include <limitless/serialization/emitter_serializer.hpp>
//#include <limitless/fx/effect_builder.hpp>
#include <limitless/instances/effect_instance.hpp>

using namespace Limitless;
using namespace Limitless::fx;

ByteBuffer EffectSerializer::serialize(const EffectInstance& instance) {
    ByteBuffer buffer;

    buffer << instance.name
           << instance.emitters;

    return buffer;
}

std::shared_ptr<EffectInstance> EffectSerializer::deserialize(Context& context, Assets& assets, const RenderSettings& settings, ByteBuffer& buffer) {
    fx::EffectBuilder builder {context, assets, settings};
    std::string name;
    size_t size;

    buffer >> name >> size;

    builder.create(name);

    for (size_t i = 0; i < size; ++i) {
        EmitterSerializer serializer;
        serializer.deserialize(context, assets, settings, buffer, builder);
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
    auto& [context, assets, settings, effect] = asset;
    effect = serializer.deserialize(context, assets, settings, buffer);
    return buffer;
}
