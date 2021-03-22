#include <limitless/loaders/material_loader.hpp>

#include <ostream>
#include <fstream>

#include <limitless/util/bytebuffer.hpp>
#include <limitless/material_system/material.hpp>
#include <limitless/serialization/material_serializer.hpp>
#include <limitless/loaders/asset_loader.hpp>

using namespace LimitlessEngine;

std::shared_ptr<Material> MaterialLoader::load(Assets& assets, const fs::path& _path) {
    auto path = convertPathSeparators(_path);
    std::ifstream stream(path, std::ios::binary | std::ios::ate);

    auto filesize = stream.tellg();
    ByteBuffer buffer{static_cast<size_t>(filesize)};

    stream.seekg(0, std::ios::beg);
    stream.read(buffer.cdata(), buffer.size());

    std::shared_ptr<Material> material;
    buffer >> AssetDeserializer<std::shared_ptr<Material>>{assets, material};
    return material;
}

void MaterialLoader::save(const fs::path& _path, const std::shared_ptr<Material>& asset) {
    auto path = convertPathSeparators(_path);
    std::ofstream stream(path, std::ios::binary);

    ByteBuffer buffer;
    buffer << *asset;

    stream.write(buffer.cdata(), buffer.size());
}
