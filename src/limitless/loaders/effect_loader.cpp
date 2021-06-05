#include <limitless/loaders/effect_loader.hpp>

#include <ostream>
#include <fstream>

#include <limitless/serialization/effect_serializer.hpp>

#include <limitless/assets.hpp>
#include <limitless/loaders/asset_manager.hpp>
#include <limitless/util/bytebuffer.hpp>
#include <limitless/instances/effect_instance.hpp>

using namespace Limitless::fx;
using namespace Limitless;

std::shared_ptr<EffectInstance> EffectLoader::load(Assets& assets, const fs::path& _path) {
    auto path = convertPathSeparators(_path);
    std::ifstream stream(path, std::ios::binary | std::ios::ate);
    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    auto filesize = stream.tellg();
    ByteBuffer buffer{static_cast<size_t>(filesize)};

    stream.seekg(0, std::ios::beg);
    stream.read(buffer.cdata(), buffer.size());

    std::shared_ptr<EffectInstance> effect;
    buffer >> AssetDeserializer<std::shared_ptr<EffectInstance>>{assets, effect};
    return effect;
}

void EffectLoader::save(const fs::path& _path, const std::shared_ptr<EffectInstance>& asset) {
    auto path = convertPathSeparators(_path);
    std::ofstream stream(path, std::ios::binary);

    ByteBuffer buffer;
    buffer << *asset;

    stream.write(buffer.cdata(), buffer.size());
}
