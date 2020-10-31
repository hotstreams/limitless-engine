#include <texture_loader.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assets.hpp>

using namespace GraphicsEngine;

std::shared_ptr<Texture> TextureLoader::load(const fs::path& path, bool bottom_left_start) {
    if (assets.textures.isExist(path.string())) {
        return assets.textures[path.string()];
    }

    stbi_set_flip_vertically_on_load(bottom_left_start);

    int width = 0, height = 0, channels = 0;
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

    if (data) {
        Texture::Format format;
        Texture::InternalFormat internal;

        switch (channels) {
            case 1:
                format = Texture::Format::Red;
                internal = Texture::InternalFormat::R8;
                break;
            case 2:
                format = Texture::Format::RG;
                internal = Texture::InternalFormat::RG8;
                break;
            case 3:
                format = Texture::Format::RGB;
                internal = Texture::InternalFormat::RGB8;
                break;
            case 4:
                format = Texture::Format::RGBA;
                internal = Texture::InternalFormat::RGBA8;
                break;
            default:
                throw std::runtime_error("Unknown number of channels.");
        }

        auto mipmap_count = glm::floor(glm::log2(static_cast<float>(glm::max(width, height)))) + 1;

        auto param_set = [] (Texture& texture) {
            texture << TexParameter<GLint>{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
                    << TexParameter<GLint>{GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR}
                    << TexParameter<GLint>{GL_TEXTURE_WRAP_S, GL_REPEAT}
                    << TexParameter<GLint>{GL_TEXTURE_WRAP_T, GL_REPEAT};
        };
        auto texture = TextureBuilder::build(Texture::Type::Tex2D, mipmap_count, internal, { width, height }, format, Texture::DataType::UnsignedByte, data, param_set);
        texture->generateMipMap();

        stbi_image_free(data);

        assets.textures.add(path.string(), texture);
        return texture;
    } else {
        throw std::runtime_error("Failed to load texture: " + path.string() + " " + stbi_failure_reason());
    }
}

std::shared_ptr<Texture> TextureLoader::loadCubemap(const fs::path &path, bool bottom_left_start) {
    stbi_set_flip_vertically_on_load(bottom_left_start);

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

    Texture::Format format;
    Texture::InternalFormat internal;

    switch (channels) {
        case 1:
            format = Texture::Format::Red;
            internal = Texture::InternalFormat::R8;
            break;
        case 2:
            format = Texture::Format::RG;
            internal = Texture::InternalFormat::RG8;
            break;
        case 3:
            format = Texture::Format::RGB;
            internal = Texture::InternalFormat::RGB8;
            break;
        case 4:
            format = Texture::Format::RGBA;
            internal = Texture::InternalFormat::RGBA8;
            break;
        default:
            throw std::runtime_error("Unknown number of channels.");
    }

    auto param_set = [] (Texture& texture) {
        texture << TexParameter<GLint>{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
                << TexParameter<GLint>{GL_TEXTURE_MIN_FILTER, GL_LINEAR}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE};
    };
    auto texture = TextureBuilder::build(Texture::Type::CubeMap, internal, { width, height }, format, Texture::DataType::UnsignedByte, data, param_set);

    for (const auto ptr : data) {
        stbi_image_free(ptr);
    }

    return texture;
}

GLFWimage TextureLoader::loadGLFWImage(const fs::path &path) {
    stbi_set_flip_vertically_on_load(false);

    int width = 0, height = 0, channels = 0;
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

    if (data) {
        return GLFWimage{ width, height, data };
    } else {
        throw std::runtime_error("Failed to load texture: " + path.string() + " " + stbi_failure_reason());
    }
}
