#include <limitless/particle_system/effect_loader.hpp>

#include <ostream>
#include <fstream>

#include <limitless/assets.hpp>
#include <limitless/asset_loader.hpp>
#include <limitless/util/bytebuffer.hpp>
#include <limitless/effect_instance.hpp>
#include <limitless/particle_system/effect_serializer.hpp>

using namespace LimitlessEngine;

std::shared_ptr<EffectInstance> EffectLoader::load(const fs::path& path) {
    std::ifstream stream(path, std::ios::binary | std::ios::ate);

    auto filesize = stream.tellg();
    ByteBuffer buffer{static_cast<size_t>(filesize)};

    stream.seekg(0, std::ios::beg);
    stream.read(buffer.cdata(), buffer.size());

    std::shared_ptr<EffectInstance> effect;
    buffer >> effect;
    return effect;
}

void EffectLoader::save(const fs::path& path, const std::string& asset_name) {
    std::ofstream stream(path, std::ios::binary);

    ByteBuffer buffer;
    buffer << *assets.effects[asset_name];

    stream.write(buffer.cdata(), buffer.size());
}
