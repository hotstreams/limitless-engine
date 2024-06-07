#include <limitless/loaders/texture_loader.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/texture/texture_builder.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
#include <limitless/assets.hpp>
#include <limitless/loaders/dds_loader.hpp>

#if GL_DEBUG
	#include <iostream>
#endif

using namespace Limitless;

namespace {
    constexpr auto ANIS_EXTENSION = "GL_EXT_texture_filter_anisotropic";
    constexpr auto S3TC_EXTENSION = "GL_EXT_texture_compression_s3tc";
    constexpr auto BPTC_EXTENSION = "GL_ARB_texture_compression_bptc";
    constexpr auto RGTC_EXTENSION = "GL_ARB_texture_compression_rgtc";
}

void TextureLoader::setFormat(Texture::Builder& builder, const TextureLoaderFlags& flags, int channels) {
    Texture::InternalFormat internal {};

    switch (flags.compression) {
        case TextureLoaderFlags::Compression::None:
        none:
            switch (channels) {
                case 1: internal = Texture::InternalFormat::R8; break;
                case 2: internal = Texture::InternalFormat::RG8; break;
                case 3: internal = (flags.space == TextureLoaderFlags::Space::sRGB) ? Texture::InternalFormat::sRGB8 : Texture::InternalFormat::RGB8; break;
                case 4: internal = (flags.space == TextureLoaderFlags::Space::sRGB) ? Texture::InternalFormat::sRGBA8 : Texture::InternalFormat::RGBA8; break;
                default: throw texture_loader_exception("Bad channels count!");
            }
            break;
        case TextureLoaderFlags::Compression::DXT1:
        dxt1:
            if (channels != 3 && channels != 4) {
                throw texture_loader_exception("Bad Compression S3TC setting for channels count!");
            }

            if (!ContextInitializer::isExtensionSupported(S3TC_EXTENSION)) {
                throw texture_loader_exception("Compression S3TC is not supported!");
            }

            switch (channels) {
                case 3: internal = (flags.space == TextureLoaderFlags::Space::sRGB) ? Texture::InternalFormat::sRGB_DXT1 : Texture::InternalFormat::RGB_DXT1; break;
                case 4: internal = (flags.space == TextureLoaderFlags::Space::sRGB) ? Texture::InternalFormat::sRGBA_DXT1 : Texture::InternalFormat::RGBA_DXT1; break;
            }
            break;
        case TextureLoaderFlags::Compression::DXT5:
        dxt5:
            if (channels != 4) {
                throw texture_loader_exception("Bad Compression S3TC setting for channels count!");
            }

            if (!ContextInitializer::isExtensionSupported(S3TC_EXTENSION)) {
                throw texture_loader_exception("Compression S3TC is not supported!");
            }

            internal = (flags.space == TextureLoaderFlags::Space::sRGB) ? Texture::InternalFormat::sRGBA_DXT5 : Texture::InternalFormat::RGBA_DXT5;
            break;
        case TextureLoaderFlags::Compression::BC7:
        bc7:
            if (channels != 3 && channels != 4) {
                throw texture_loader_exception("Bad Compression BPTC setting for channels count!");
            }

            if (!ContextInitializer::isExtensionSupported(BPTC_EXTENSION)) {
                throw texture_loader_exception("Compression BPTC is not supported!");
            }

            internal = (flags.space == TextureLoaderFlags::Space::sRGB) ? Texture::InternalFormat::sRGBA_BC7 : Texture::InternalFormat::RGBA_BC7;
            break;
        case TextureLoaderFlags::Compression::RGTC:
        rgtc:
            if (channels != 1 && channels != 2) {
                throw texture_loader_exception("Bad Compression RGTC setting for channels count!");
            }

            if (!ContextInitializer::isExtensionSupported(RGTC_EXTENSION)) {
                throw texture_loader_exception("Compression RGTC is not supported!");
            }

            switch (channels) {
                case 1: internal = Texture::InternalFormat::R_RGTC; break;
                case 2: internal = Texture::InternalFormat::RG_RGTC; break;
            }
            break;
        case TextureLoaderFlags::Compression::Default:
            if ((channels == 1 || channels == 2) && ContextInitializer::isExtensionSupported(RGTC_EXTENSION)) {
                goto rgtc;
            }

            if ((channels == 3 || channels == 4) && ContextInitializer::isExtensionSupported(BPTC_EXTENSION)) {
                goto bc7;
            }

            if (channels == 3 && ContextInitializer::isExtensionSupported(S3TC_EXTENSION)) {
                goto dxt1;
            }

            if (channels == 4 && ContextInitializer::isExtensionSupported(S3TC_EXTENSION)) {
                goto dxt5;
            }

            goto none;
    }

    Texture::Format format {};
    switch (channels) {
        case 1: format = Texture::Format::Red; break;
        case 2: format = Texture::Format::RG; break;
        case 3: format = Texture::Format::RGB; break;
        case 4: format = Texture::Format::RGBA; break;
        default: throw texture_loader_exception("Bad channels count!");
    }

    builder.internal_format(internal)
            .format(format);
}

void TextureLoader::setAnisotropicFilter(const std::shared_ptr<Texture>& texture, const TextureLoaderFlags& flags) {
    if (flags.anisotropic_filter && ContextInitializer::isExtensionSupported(ANIS_EXTENSION)) {
        if (flags.anisotropic_value == 0.0f) {
            texture->setAnisotropicFilterMax();
        } else {
            texture->setAnisotropicFilter(flags.anisotropic_value);
        }
    }
}

std::shared_ptr<Texture> TextureLoader::load(Assets& assets, const fs::path& _path, const TextureLoaderFlags& flags) {
    auto path = convertPathSeparators(_path);

    if (assets.textures.contains(path.stem().string())) {
        return assets.textures[path.stem().string()];
    }

    if (path.extension().string() == ".dds") {
    	return DDSLoader::load(assets, path, flags);
    }

    stbi_set_flip_vertically_on_load(static_cast<bool>((int)flags.origin));

    int width = 0, height = 0, channels = 0;
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

    if (!data) {
	    throw std::runtime_error("Failed to load texture: " + path.string() + " " + stbi_failure_reason());
    }

	#if GL_DEBUG
        if (!isPowerOfTwo(width, height)) {
        	std::cerr << path.string() << " has not 2^n size, its not recommended to have it!" << std::endl;
        }
	#endif

    setDownScale(width, height, channels, data, flags);

    Texture::Builder builder = Texture::builder();

    builder.target(Texture::Type::Tex2D)
            .levels(glm::floor(glm::log2(static_cast<float>(glm::max(width, height)))) + 1)
            .size({width, height})
            .data_type(Texture::DataType::UnsignedByte)
            .data(data)
            .path(path);

    setFormat(builder, flags, channels);
    setTextureParameters(builder, flags);

    auto texture = builder.build();
    setAnisotropicFilter(texture, flags);

    if (flags.downscale != TextureLoaderFlags::DownScale::None) {
        delete data;
    } else {
        stbi_image_free(data);
    }

    assets.textures.add(path.stem().string(), texture);
    return texture;
}

std::shared_ptr<Texture> TextureLoader::loadCubemap([[maybe_unused]] Assets& assets, const fs::path& _path, const TextureLoaderFlags& flags) {
    auto path = convertPathSeparators(_path);

    stbi_set_flip_vertically_on_load(static_cast<bool>((int)flags.origin));

    constexpr std::array ext = { "_right", "_left", "_top", "_bottom", "_front", "_back" };

    std::array<void*, 6> data = { nullptr };
    int width = 0, height = 0, channels = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        std::string p = path.parent_path().string() + PATH_SEPARATOR + path.stem().string() + ext[i] + path.extension().string();
        data[i] = stbi_load(p.c_str(), &width, &height, &channels, 0);

        if (!data[i]) {
            throw std::runtime_error("Failed to load texture: " + path.string() + " " + stbi_failure_reason());
        }
    }

    Texture::Builder builder = Texture::builder();

    builder.target(Texture::Type::CubeMap)
            .size(glm::uvec2{width, height})
            .data_type(Texture::DataType::UnsignedByte)
            .data(data);

    setFormat(builder, flags, channels);
    setTextureParameters(builder, flags);

    builder.wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .wrap_r(Texture::Wrap::ClampToEdge);

    builder.path(path);
    auto texture = builder.buildMutable();
    setAnisotropicFilter(texture, flags);

    std::for_each(data.begin(), data.end(), [] (auto* ptr) { stbi_image_free(ptr); });

    return texture;
}

GLFWimage TextureLoader::loadGLFWImage([[maybe_unused]] Assets& assets, const fs::path& _path, const TextureLoaderFlags& flags) {
    auto path = convertPathSeparators(_path);

    stbi_set_flip_vertically_on_load(static_cast<bool>(flags.origin));

    int width = 0, height = 0, channels = 0;
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

    if (data) {
        return GLFWimage{ width, height, data };
    } else {
        throw std::runtime_error("Failed to load texture: " + path.string() + " " + stbi_failure_reason());
    }
}

void TextureLoader::setDownScale(int& width, int& height, int channels, unsigned char*& data, const TextureLoaderFlags& flags) {
	if (flags.downscale == TextureLoaderFlags::DownScale::None) {
		return;
	}

	if (!isPowerOfTwo(width, height)) {
		throw texture_loader_exception("stbimage cannot resize not power of two texture!");
	}

	int scaled_width, scaled_height;
    unsigned char* resized_data;

    scaled_width = width >> static_cast<uint32_t>(flags.downscale);
	scaled_height = height >> static_cast<uint32_t>(flags.downscale);

    resized_data = new unsigned char[channels * scaled_height * scaled_width];
    stbir_resize_uint8(data, width, height, 0, resized_data, scaled_width, scaled_height, 0, channels);

    width = scaled_width;
    height = scaled_height;

    stbi_image_free(data);
    data = resized_data;
}

void TextureLoader::setTextureParameters(Texture::Builder& builder, const TextureLoaderFlags& flags) {
    builder.mipmap(flags.mipmap)
            .wrap_s(flags.wrapping)
            .wrap_t(flags.wrapping)
            .wrap_r(flags.wrapping);

    switch (flags.filter) {
        case TextureLoaderFlags::Filter::Linear:
            builder.mag_filter(Texture::Filter::Linear);
            builder.min_filter(Texture::Filter::Linear);
            if (flags.mipmap) {
                builder.min_filter(Texture::Filter::LinearMipmapLinear);
            }
            break;
        case TextureLoaderFlags::Filter::Nearest:
            builder.mag_filter(Texture::Filter::Nearest);
            builder.min_filter(Texture::Filter::Nearest);
            if (flags.mipmap) {
                builder.min_filter(Texture::Filter::NearestMipmapNearest);
            }
            break;
    }
}

bool TextureLoader::isPowerOfTwo(int width, int height) {
	return ((width != 0) && !(width & (width - 1))) && ((height != 0) && !(height & (height - 1)));
}

std::shared_ptr<Texture> TextureLoader::load(Assets &assets, const std::vector<fs::path> &paths, const TextureLoaderFlags &flags) {
    //TODO: size equality check
    Texture::Builder builder = Texture::builder();

    for (const auto& _path: paths) {
        auto path = convertPathSeparators(_path);

//        if (assets.textures.contains(path.stem().string())) {
//            return assets.textures[path.stem().string()];
//        }

//        if (path.extension().string() == ".dds") {
//            return DDSLoader::load(assets, path, flags);
//        }

        stbi_set_flip_vertically_on_load(static_cast<bool>((int)flags.origin));

        int width = 0, height = 0, channels = 0;
        unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

        if (!data) {
            throw std::runtime_error("Failed to load texture: " + path.string() + " " + stbi_failure_reason());
        }

        #if GL_DEBUG
                if (!isPowerOfTwo(width, height)) {
                    std::cerr << path.string() << " has not 2^n size, its not recommended to have it!" << std::endl;
                }
        #endif

        setDownScale(width, height, channels, data, flags);

        builder.target(Texture::Type::Tex2D)
                .levels(glm::floor(glm::log2(static_cast<float>(glm::max(width, height)))) + 1)
                .size({width, height})
                .data_type(Texture::DataType::UnsignedByte)
                .layer_data(data)
                .path(path);

        setFormat(builder, flags, channels);
        setTextureParameters(builder, flags);
    }

    auto texture = builder.build();
    setAnisotropicFilter(texture, flags);

    //TODO: restore leak
//    if (flags.downscale != TextureLoaderFlags::DownScale::None) {
//        delete data;
//    } else {
//        stbi_image_free(data);
//    }

//    assets.textures.add(path.stem().string(), texture);
    return texture;
}
