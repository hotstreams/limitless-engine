#include <material_system/material_loader.hpp>

#include <ostream>
#include <fstream>

#include <assets.hpp>
#include <util/bytebuffer.hpp>
#include <material_system/material.hpp>
#include <material_system/material_serializer.hpp>
#include "asset_loader.hpp"

using namespace LimitlessEngine;

std::shared_ptr<Material> MaterialLoader::load(const std::string& asset_name) {
    std::ifstream stream(ASSETS_DIR "materials/" + asset_name, std::ios::binary | std::ios::ate);

    auto filesize = stream.tellg();
    ByteBuffer buffer{static_cast<size_t>(filesize)};

    stream.seekg(0, std::ios::beg);
    stream.read(buffer.cdata(), buffer.size());

    std::shared_ptr<Material> material;
    buffer >> material;
    return material;
}

void MaterialLoader::save(std::string asset_name) {
    std::ofstream stream(ASSETS_DIR "materials/" + asset_name, std::ios::binary);

    ByteBuffer buffer;
    buffer << *assets.materials[asset_name];

    stream.write(buffer.cdata(), buffer.size());
}
