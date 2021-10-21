#include <limitless/loaders/dds_loader.hpp>
#include <limitless/loaders/texture_loader.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/util/filesystem.hpp>
#include <fstream>
#include <iostream>

using namespace Limitless;

namespace {
    class DDSPIXELFORMAT {
    public:
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwFourCC;
        uint32_t dwRGBBitCount;
        uint32_t dwRBitMask;
        uint32_t dwGBitMask;
        uint32_t dwBBitMask;
        uint32_t dwABitMask;
    };

    class DDSHEADER {
    public:
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwHeight;
        uint32_t dwWidth;
        uint32_t dwPitchOrLinearSize;
        uint32_t dwDepth;
        uint32_t dwMipMapCount;
        uint32_t dwReserved1[11];
        DDSPIXELFORMAT ddspf;
        uint32_t dwCaps1;
        uint32_t dwCaps2;
        uint32_t dwReserved2[3];
    };

    constexpr auto DDS_CODE = "DDS ";
    constexpr auto DXT1_CODE = 0x31545844;
    constexpr auto DXT3_CODE = 0x33545844;
    constexpr auto DXT5_CODE = 0x35545844;
    constexpr auto DXT1_BLOCK_SIZE = 8;
    constexpr auto DXT5_BLOCK_SIZE = 16;
}

std::size_t DDSLoader::getDXTByteCount(glm::uvec2 size, std::size_t block_size) noexcept {
    const auto s = (size + 3u) / 4u;
    return s.x * s.y * block_size;
}

void DDSLoader::loadLevel(std::shared_ptr<Texture>& texture, std::ifstream& fs, uint32_t level, int channels) {
    const auto s = glm::clamp(texture->getSize() >> level, 1u, std::numeric_limits<uint32_t>::max());
    const auto byte_count = getDXTByteCount(s, channels == 3 ? DXT1_BLOCK_SIZE : DXT5_BLOCK_SIZE);

    auto* data = new uint8_t[byte_count];
    fs.read(reinterpret_cast<char *>(data), byte_count);
    texture->compressedImage(level, static_cast<glm::uvec2>(s), data, byte_count);
    delete[] data;
}

std::shared_ptr<Texture> DDSLoader::load(Assets& assets, const fs::path& _path, const TextureLoaderFlags& flags) {
    auto path = convertPathSeparators(_path);

    if (assets.textures.contains(path.stem().string())) {
        return assets.textures[path.stem().string()];
    }

    std::ifstream fs;
	fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		fs.open(path, std::ios::binary);
	} catch (std::exception& e) {
		throw dds_loader_exception("Cant open " + path.string());
	}

    std::array<char, 4> code {0};
    fs.read(code.data(), code.size());
    if (std::string(code.data(), code.size()) != DDS_CODE) {
        throw dds_loader_exception("It is not a DDS file!");
    }

    DDSHEADER header {};
    fs.read(reinterpret_cast<char*>(&header), sizeof(DDSHEADER));

    TextureBuilder builder;
    builder .setTarget(Texture::Type::Tex2D)
            .setSize({header.dwWidth, header.dwHeight});

    uint32_t channels {};
    switch (header.ddspf.dwFourCC) {
        case DXT1_CODE:
            builder.setInternalFormat(Texture::InternalFormat::RGB_DXT1);
            channels = 3;
            break;
	    case DXT3_CODE:
		    builder.setInternalFormat(Texture::InternalFormat::RGBA_DXT3);
		    channels = 4;
		    break;
        case DXT5_CODE:
            builder.setInternalFormat(Texture::InternalFormat::RGBA_DXT5);
            channels = 4;
            break;
        default:
            throw dds_loader_exception("Unsupported compression code. Contact the admin!");
    }

    const auto byte_count = getDXTByteCount({header.dwWidth, header.dwHeight}, channels == 3 ? DXT1_BLOCK_SIZE : DXT5_BLOCK_SIZE);
    auto* data = new uint8_t[byte_count];
    fs.read(reinterpret_cast<char*>(data), byte_count);
    builder.setCompressedData(data, byte_count);
    TextureLoader::setTextureParameters(builder, flags);
    builder.setPath(path);
    auto texture = builder.buildMutable();
    delete[] data;

    if (flags.mipmap) {
        auto mipmap_count = header.dwMipMapCount != 0 ? header.dwMipMapCount - 1 : header.dwMipMapCount;
        for (uint32_t i = 0; i < mipmap_count; ++i) {
            loadLevel(texture, fs, i + 1, channels);
        }
    }

    assets.textures.add(path.stem().string(), texture);
    return texture;
}

