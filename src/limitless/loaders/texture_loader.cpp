#include <limitless/loaders/texture_loader.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/texture/texture_builder.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <limitless/assets.hpp>
#include <limitless/loaders/dds_loader.hpp>

#if LIMITLESS_OPENGL_DEBUG
	#include <iostream>
#endif

using namespace Limitless;

namespace {
    constexpr auto ANIS_EXTENSION = "GL_EXT_texture_filter_anisotropic";
    constexpr auto S3TC_EXTENSION = "GL_EXT_texture_compression_s3tc";
    constexpr auto BPTC_EXTENSION = "GL_ARB_texture_compression_bptc";
    constexpr auto RGTC_EXTENSION = "GL_ARB_texture_compression_rgtc";
    constexpr auto ASTC_EXTENSION = "GL_KHR_texture_compression_astc_ldr";

    constexpr float kMinAlphaCoveragePow = 0.05f;
    constexpr float kMaxAlphaCoveragePow = 8.0f;

    static float computeAlphaCoverage(const std::vector<float>& alpha, float cutoff) {
        if (alpha.empty()) return 0.0f;
        size_t count = 0;
        for (float a : alpha) {
            if (a > cutoff) ++count;
        }
        return static_cast<float>(count) / static_cast<float>(alpha.size());
    }

    static float computeAlphaCoveragePow(const std::vector<float>& alpha, float cutoff, float p) {
        if (alpha.empty()) return 0.0f;
        size_t count = 0;
        for (float a : alpha) {
            // pow(0, p) == 0; pow(1, p) == 1.
            const float ap = std::pow(a, p);
            if (ap > cutoff) ++count;
        }
        return static_cast<float>(count) / static_cast<float>(alpha.size());
    }

    static float solveAlphaPowForCoverage(const std::vector<float>& alpha, float cutoff, float target_coverage) {
        // Coverage is monotonic decreasing with p: smaller p boosts alpha (more coverage).
        float lo = kMinAlphaCoveragePow;
        float hi = kMaxAlphaCoveragePow;

        const float cov_lo = computeAlphaCoveragePow(alpha, cutoff, lo);
        const float cov_hi = computeAlphaCoveragePow(alpha, cutoff, hi);

        if (target_coverage >= cov_lo) {
            return lo;
        }
        if (target_coverage <= cov_hi) {
            return hi;
        }

        // Binary search for p such that coverage ~= target.
        for (int i = 0; i < 16; ++i) {
            const float mid = 0.5f * (lo + hi);
            const float cov_mid = computeAlphaCoveragePow(alpha, cutoff, mid);
            if (cov_mid > target_coverage) {
                // too much coverage -> increase p (reduce alpha)
                lo = mid;
            } else {
                hi = mid;
            }
        }
        return 0.5f * (lo + hi);
    }

    static void applyAlphaPowToMip(std::vector<uint8_t>& rgba, float p) {
        // rgba is 4-channel uint8
        for (size_t i = 3; i < rgba.size(); i += 4) {
            const float a = static_cast<float>(rgba[i]) / 255.0f;
            const float ap = std::pow(a, p);
            const int ai = static_cast<int>(ap * 255.0f + 0.5f);
            rgba[i] = static_cast<uint8_t>(std::clamp(ai, 0, 255));
        }
    }

    static std::vector<float> extractAlpha01(const std::vector<uint8_t>& rgba) {
        std::vector<float> alpha;
        alpha.reserve(rgba.size() / 4);
        for (size_t i = 3; i < rgba.size(); i += 4) {
            alpha.emplace_back(static_cast<float>(rgba[i]) / 255.0f);
        }
        return alpha;
    }

    static void generateCoveragePreservingMipChain(
        std::vector<std::vector<uint8_t>>& out_mips,
        int width,
        int height,
        const uint8_t* level0_rgba,
        const TextureLoaderFlags& flags
    ) {
        // Only supports RGBA8 input for now.
        const int channels = 4;
        const int levels = static_cast<int>(std::floor(std::log2(static_cast<float>(std::max(width, height))))) + 1;
        out_mips.clear();
        out_mips.resize(levels);

        out_mips[0].assign(level0_rgba, level0_rgba + (width * height * channels));

        // Target coverage from the top mip.
        const float cutoff = flags.alpha_coverage_cutoff;
        const float target_coverage = computeAlphaCoverage(extractAlpha01(out_mips[0]), cutoff);

        int prev_w = width;
        int prev_h = height;

        for (int level = 1; level < levels; ++level) {
            const int w = std::max(1, prev_w / 2);
            const int h = std::max(1, prev_h / 2);
            out_mips[level].resize(w * h * channels);

            const unsigned char* src = out_mips[level - 1].data();
            unsigned char* dst = out_mips[level].data();

            if (flags.space == TextureLoaderFlags::Space::sRGB) {
                // Correct sRGB downsample for RGB, linear for alpha.
                stbir_resize_uint8_srgb(
                    src, prev_w, prev_h, 0,
                    dst, w, h, 0,
                    channels, 3, 0
                );
            } else {
                stbir_resize_uint8(
                    src, prev_w, prev_h, 0,
                    dst, w, h, 0,
                    channels
                );
            }

            // Adjust alpha in this mip to preserve coverage w.r.t. alpha cutoff.
            auto alpha = extractAlpha01(out_mips[level]);
            const float p = solveAlphaPowForCoverage(alpha, cutoff, target_coverage);
            applyAlphaPowToMip(out_mips[level], p);

            prev_w = w;
            prev_h = h;
        }
    }
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
            // RGTC is good for normals / masks, bad for color.
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

	#if LIMITLESS_OPENGL_DEBUG
        if (!isPowerOfTwo(width, height)) {
        	std::cerr << path.string() << " size is not a power of 2, please resize!" << std::endl;
        }
	#endif

    setDownScale(width, height, channels, data, flags);

    // Special path: generate coverage-preserving mipmaps for alpha-cutout foliage.
    // We do it on CPU because GPU mipmap generation (box filter) causes coverage loss at distance.
    if (flags.mipmap && flags.preserve_alpha_coverage && channels == 4 && flags.downscale == TextureLoaderFlags::DownScale::None) {
        std::vector<std::vector<uint8_t>> mips;
        generateCoveragePreservingMipChain(mips, width, height, data, flags);

        // We no longer need original stb buffer.
        stbi_image_free(data);

        Texture::Builder builder = Texture::builder();
        builder.target(Texture::Type::Tex2D)
            .levels(static_cast<uint32_t>(mips.size()))
            .size({width, height})
            .data_type(Texture::DataType::UnsignedByte)
            .data(mips[0].data())
            .path(path);

        // Disable GPU mip generation; we upload mips ourselves.
        auto local_flags = flags;
        local_flags.mipmap = false;

        setFormat(builder, local_flags, channels);
        setTextureParameters(builder, local_flags);

        // Force mipmapped min filter even though local_flags.mipmap=false (we provide mip levels manually).
        switch (flags.filter) {
            case TextureLoaderFlags::Filter::Linear:
                builder.min_filter(Texture::Filter::LinearMipmapLinear);
                builder.mag_filter(Texture::Filter::Linear);
                break;
            case TextureLoaderFlags::Filter::Nearest:
                builder.min_filter(Texture::Filter::NearestMipmapNearest);
                builder.mag_filter(Texture::Filter::Nearest);
                break;
        }

        // Disable compression for this path (we upload raw mip levels).
        builder.internal_format((flags.space == TextureLoaderFlags::Space::sRGB) ? Texture::InternalFormat::sRGBA8 : Texture::InternalFormat::RGBA8)
               .format(Texture::Format::RGBA);

        auto texture = builder.build();

        // Upload remaining mip levels.
        int w = width;
        int h = height;
        for (size_t level = 1; level < mips.size(); ++level) {
            w = std::max(1, w / 2);
            h = std::max(1, h / 2);
            if (texture->isImmutable()) {
                texture->subImage(static_cast<uint32_t>(level), {0, 0}, {static_cast<uint32_t>(w), static_cast<uint32_t>(h)}, mips[level].data());
            } else {
                texture->image(static_cast<uint32_t>(level), {static_cast<uint32_t>(w), static_cast<uint32_t>(h)}, mips[level].data());
            }
        }

        setAnisotropicFilter(texture, flags);
        assets.textures.add(path.stem().string(), texture);
        return texture;
    }

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

std::shared_ptr<Texture> TextureLoader::load(Assets& assets, const std::string& name, const uint8_t* buffer, size_t size, const TextureLoaderFlags& flags) {
    stbi_set_flip_vertically_on_load(static_cast<bool>((int)flags.origin));

    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* data = stbi_load_from_memory(buffer, size, &width, &height, &channels, 0);

    if (!data) {
        throw std::runtime_error("Failed to load texture " + name + " from memory: " + stbi_failure_reason());
    }

    setDownScale(width, height, channels, data, flags);

    if (flags.mipmap && flags.preserve_alpha_coverage && channels == 4 && flags.downscale == TextureLoaderFlags::DownScale::None) {
        std::vector<std::vector<uint8_t>> mips;
        generateCoveragePreservingMipChain(mips, width, height, data, flags);
        stbi_image_free(data);

        Texture::Builder builder = Texture::builder();
        builder.target(Texture::Type::Tex2D)
            .levels(static_cast<uint32_t>(mips.size()))
            .size({width, height})
            .data_type(Texture::DataType::UnsignedByte)
            .data(mips[0].data());

        auto local_flags = flags;
        local_flags.mipmap = false;

        setFormat(builder, local_flags, channels);
        setTextureParameters(builder, local_flags);

        switch (flags.filter) {
            case TextureLoaderFlags::Filter::Linear:
                builder.min_filter(Texture::Filter::LinearMipmapLinear);
                builder.mag_filter(Texture::Filter::Linear);
                break;
            case TextureLoaderFlags::Filter::Nearest:
                builder.min_filter(Texture::Filter::NearestMipmapNearest);
                builder.mag_filter(Texture::Filter::Nearest);
                break;
        }

        builder.internal_format((flags.space == TextureLoaderFlags::Space::sRGB) ? Texture::InternalFormat::sRGBA8 : Texture::InternalFormat::RGBA8)
               .format(Texture::Format::RGBA);

        auto texture = builder.build();

        int w = width;
        int h = height;
        for (size_t level = 1; level < mips.size(); ++level) {
            w = std::max(1, w / 2);
            h = std::max(1, h / 2);
            if (texture->isImmutable()) {
                texture->subImage(static_cast<uint32_t>(level), {0, 0}, {static_cast<uint32_t>(w), static_cast<uint32_t>(h)}, mips[level].data());
            } else {
                texture->image(static_cast<uint32_t>(level), {static_cast<uint32_t>(w), static_cast<uint32_t>(h)}, mips[level].data());
            }
        }

        setAnisotropicFilter(texture, flags);
        assets.textures.add(name, texture);
        return texture;
    }

    Texture::Builder builder = Texture::builder();

    builder.target(Texture::Type::Tex2D)
            .levels(glm::floor(glm::log2(static_cast<float>(glm::max(width, height)))) + 1)
            .size({width, height})
            .data_type(Texture::DataType::UnsignedByte)
            .data(data);

    setFormat(builder, flags, channels);
    setTextureParameters(builder, flags);

    auto texture = builder.build();
    setAnisotropicFilter(texture, flags);

    if (flags.downscale != TextureLoaderFlags::DownScale::None) {
        delete data;
    } else {
        stbi_image_free(data);
    }

    assets.textures.add(name, texture);
    return texture;
}

std::shared_ptr<Texture> TextureLoader::loadCubemap([[maybe_unused]] Assets& assets, const fs::path& path, const TextureLoaderFlags& flags) {
    constexpr std::array ext = { "_right", "_left", "_top", "_bottom", "_front", "_back" };

    size_t i = 0;
    std::array<fs::path, 6> paths;
    std::generate(
        std::begin(paths),
        std::end(paths),
        [&]() {
            return path.parent_path() / (path.stem().string() + ext[i++] + path.extension().string());
        }
    );
    return loadCubemap(assets, paths, flags);
}

std::shared_ptr<Texture> TextureLoader::loadCubemap(Assets& assets, const std::array<fs::path, 6>& paths, const TextureLoaderFlags& flags) {
    stbi_set_flip_vertically_on_load(static_cast<bool>((int)flags.origin));

    std::array<void*, 6> data = { nullptr };
    int width = 0, height = 0, channels = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        std::string p = convertPathSeparators(paths[i]).string();
        data[i] = stbi_load(p.c_str(), &width, &height, &channels, 0);

        if (!data[i]) {
            throw texture_loader_exception("Failed to load texture: " + p + " " + stbi_failure_reason());
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

std::shared_ptr<Texture> TextureLoader::load([[maybe_unused]] Assets &assets, const std::vector<fs::path> &paths, const TextureLoaderFlags &flags) {
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

        builder.target(Texture::Type::Tex2DArray)
                .levels(glm::floor(glm::log2(static_cast<float>(glm::max(width, height)))) + 1)
                .size({width, height})
                .data_type(Texture::DataType::UnsignedByte)
                .layer_data(data)
                .path(path);

        setFormat(builder, flags, channels);
        setTextureParameters(builder, flags);
    }

    auto texture = builder.buildMutable();
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

CpuImageBytes TextureLoader::load_image_cpu(const fs::path& _path, const TextureLoaderFlags& flags) {
	const auto path = convertPathSeparators(_path);
	stbi_set_flip_vertically_on_load(static_cast<bool>((int)flags.origin));
	int w = 0;
	int h = 0;
	int c = 0;
	unsigned char* data = stbi_load(path.string().c_str(), &w, &h, &c, 0);
	if (!data) {
		throw texture_loader_exception(
			std::string("load_image_cpu failed: ") + path.string() + " "
			+ (stbi_failure_reason() ? stbi_failure_reason() : "")
		);
	}
	CpuImageBytes out;
	out.width = w;
	out.height = h;
	out.channels = c;
	out.data.assign(data, data + static_cast<size_t>(w) * static_cast<size_t>(h) * static_cast<size_t>(c));
	stbi_image_free(data);
	return out;
}

CpuImageBytes TextureLoader::load_image_cpu(const uint8_t* buffer, size_t size, const TextureLoaderFlags& flags) {
	stbi_set_flip_vertically_on_load(static_cast<bool>((int)flags.origin));
	int w = 0;
	int h = 0;
	int c = 0;
	unsigned char* data = stbi_load_from_memory(buffer, static_cast<int>(size), &w, &h, &c, 0);
	if (!data) {
		throw texture_loader_exception(
			std::string("load_image_cpu (memory) failed: ")
			+ (stbi_failure_reason() ? stbi_failure_reason() : "")
		);
	}
	CpuImageBytes out;
	out.width = w;
	out.height = h;
	out.channels = c;
	out.data.assign(data, data + static_cast<size_t>(w) * static_cast<size_t>(h) * static_cast<size_t>(c));
	stbi_image_free(data);
	return out;
}

bool TextureLoader::save_png(
	const fs::path& _path, int width, int height, int channels, const uint8_t* pixels, int stride_bytes
) {
	if (width <= 0 || height <= 0 || channels <= 0 || pixels == nullptr) {
		return false;
	}
	const auto path = convertPathSeparators(_path);
	const int stride = stride_bytes > 0 ? stride_bytes : width * channels;
	return stbi_write_png(path.string().c_str(), width, height, channels, pixels, stride) != 0;
}

CpuImageBytes TextureLoader::resize_image_cpu(
	const CpuImageBytes& src, int dst_width, int dst_height, bool linear_colorspace
) {
	if (src.channels < 1 || src.channels > 4) {
		throw texture_loader_exception("resize_image_cpu: unsupported channel count");
	}
	if (dst_width <= 0 || dst_height <= 0) {
		throw texture_loader_exception("resize_image_cpu: invalid destination size");
	}
	CpuImageBytes out;
	out.width = dst_width;
	out.height = dst_height;
	out.channels = src.channels;
	out.data.resize(static_cast<size_t>(dst_width) * static_cast<size_t>(dst_height) * static_cast<size_t>(src.channels));
	const int src_stride = src.width * src.channels;
	const int dst_stride = dst_width * src.channels;
	int ok = 0;
	if (linear_colorspace) {
		ok = stbir_resize_uint8(
			src.data.data(),
			src.width,
			src.height,
			src_stride,
			out.data.data(),
			dst_width,
			dst_height,
			dst_stride,
			src.channels
		);
	} else {
		const int alpha = src.channels == 4 ? 3 : -1;
		ok = stbir_resize_uint8_srgb(
			src.data.data(),
			src.width,
			src.height,
			src_stride,
			out.data.data(),
			dst_width,
			dst_height,
			dst_stride,
			src.channels,
			alpha,
			0
		);
	}
	if (!ok) {
		throw texture_loader_exception("resize_image_cpu: stbir_resize failed");
	}
	return out;
}
