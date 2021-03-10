#include <material_system/material_loader.hpp>

#include <ostream>
#include <assets.hpp>
#include <util/bytebuffer.hpp>
#include <fstream>

using namespace LimitlessEngine;

std::shared_ptr<Material> MaterialLoader::load(std::string_view asset_name) {
    std::ifstream stream(ASSETS_DIR "materials/" + asset_name, std::ios::binary | std::ios::ate);

    auto p = stream.tellg();
    ByteBuffer buffer{p};

    stream.seekg(0, std::ios::beg);
    stream.read(buffer.begin(), buffer.getSize());

    std::shared_ptr<Material> material;
    buffer >> material;
    return material;
}

void MaterialLoader::save(std::string_view asset_name) {
    std::ofstream stream(ASSETS_DIR "materials/" + asset_name, std::ios::binary);

    ByteBuffer buffer;
    buffer << assets.materials[asset_name];

    stream.write(buffer.begin(), buffer.getSize());
}