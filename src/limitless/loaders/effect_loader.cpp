#include <limitless/loaders/effect_loader.hpp>

#include <ostream>
#include <fstream>

#include <limitless/assets.hpp>
#include <limitless/loaders/asset_loader.hpp>
#include <limitless/util/bytebuffer.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/serialization/effect_serializer.hpp>

using namespace LimitlessEngine;

std::shared_ptr<EffectInstance> EffectLoader::load(Assets& assets, const fs::path& _path) {
    auto path = convertPathSeparators(_path);
    std::ifstream stream(path, std::ios::binary | std::ios::ate);

    auto filesize = stream.tellg();
    ByteBuffer buffer{static_cast<size_t>(filesize)};

    stream.seekg(0, std::ios::beg);
    stream.read(buffer.cdata(), buffer.size());

    std::shared_ptr<EffectInstance> effect;
    buffer >> std::pair<Assets&, std::shared_ptr<EffectInstance>&>{assets, effect};
    return effect;
}

void EffectLoader::save(const fs::path& _path, const std::shared_ptr<EffectInstance>& asset) {
    auto path = convertPathSeparators(_path);
    std::ofstream stream(path, std::ios::binary);

    ByteBuffer buffer;
    buffer << *asset;

    stream.write(buffer.cdata(), buffer.size());
}
