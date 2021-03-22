#include <limitless/material_system/material_loader.hpp>

#include <ostream>
#include <fstream>

#include <limitless/assets.hpp>
#include <limitless/util/bytebuffer.hpp>
#include <limitless/material_system/material.hpp>
#include <limitless/material_system/material_serializer.hpp>
#include <limitless/asset_loader.hpp>

using namespace LimitlessEngine;

std::shared_ptr<Material> MaterialLoader::load(const fs::path& path) {
    std::ifstream stream(path, std::ios::binary | std::ios::ate);

    auto filesize = stream.tellg();
    ByteBuffer buffer{static_cast<size_t>(filesize)};

    stream.seekg(0, std::ios::beg);
    stream.read(buffer.cdata(), buffer.size());

    std::shared_ptr<Material> material;
    buffer >> material;
    return material;
}

void MaterialLoader::save(const fs::path& path, const std::string& asset_name) {
    std::ofstream stream(path, std::ios::binary);

    ByteBuffer buffer;
    buffer << *assets.materials[asset_name];

    stream.write(buffer.cdata(), buffer.size());
}
