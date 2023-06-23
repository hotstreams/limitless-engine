#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"

#include <limitless/core/context.hpp>
#include <limitless/core/texture_builder.hpp>
#include <iostream>

using namespace Limitless;
using namespace LimitlessTest;

class TextureChecker {
public:
    static GLint getMagFilter(GLenum target) {
        GLint data;
        glGetTexParameteriv(target, GL_TEXTURE_MAG_FILTER, &data);
        return data;
    }

    static GLint getMinFilter(GLenum target) {
        GLint data;
        glGetTexParameteriv(target, GL_TEXTURE_MIN_FILTER, &data);
        return data;
    }

    static GLint getMinLOD(GLenum target) {
        GLint data;
        glGetTexParameteriv(target, GL_TEXTURE_MIN_LOD, &data);
        return data;
    }

    static GLint getMaxLOD(GLenum target) {
        GLint data;
        glGetTexParameteriv(target, GL_TEXTURE_MAX_LOD, &data);
        return data;
    }

    static GLint getBaseMipMapLevel(GLenum target) {
        GLint data;
        glGetTexParameteriv(target, GL_TEXTURE_BASE_LEVEL, &data);
        return data;
    }

    static GLint getMaxMipMapLevel(GLenum target) {
        GLint data;
        glGetTexParameteriv(target, GL_TEXTURE_MAX_LEVEL, &data);
        return data;
    }

    static GLint getWrapS(GLenum target) {
        GLint data;
        glGetTexParameteriv(target, GL_TEXTURE_WRAP_S, &data);
        return data;
    }

    static GLint getWrapT(GLenum target) {
        GLint data;
        glGetTexParameteriv(target, GL_TEXTURE_WRAP_T, &data);
        return data;
    }

    static GLint getWrapR(GLenum target) {
        GLint data;
        glGetTexParameteriv(target, GL_TEXTURE_WRAP_R, &data);
        return data;
    }

    static GLfloat getMaxAnisotropy(GLenum target) {
        GLfloat data;
        glGetTexParameterfv(target, GL_TEXTURE_MAX_ANISOTROPY, &data);
        return data;
    }

    static glm::vec4 getBorderColor(GLenum target) {
        GLfloat data[4];
        glGetTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, data);
        return {data[0], data[1], data[2], data[3]};
    }

    static bool isImmutable(GLenum target) {
        GLint data;
        glGetTexParameteriv(target, GL_TEXTURE_IMMUTABLE_FORMAT, &data);
        return data;
    }

    static glm::uvec3 getSize(GLenum target) {
        GLint width;
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
        GLint height;
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);
        GLint depth;
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_DEPTH, &depth);
        return { width, height, depth };
    }

    static GLint getInternalFormat(GLenum target) {
        GLint data;
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_INTERNAL_FORMAT, &data);
        return data;
    }

    static GLint isCompressed(GLenum target) {
        GLint data;
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_COMPRESSED, &data);
        return data;
    }
};

void check_texture_state(const std::shared_ptr<Texture>& texture) {
    texture->bind(0);
    REQUIRE(TextureChecker::getWrapS((GLenum)texture->getType()) == (GLenum)texture->getWrapS());
    REQUIRE(TextureChecker::getWrapT((GLenum)texture->getType()) == (GLenum)texture->getWrapT());
    REQUIRE(TextureChecker::getWrapR((GLenum)texture->getType()) == (GLenum)texture->getWrapR());

    REQUIRE(TextureChecker::getMagFilter((GLenum)texture->getType()) == (GLenum)texture->getMag());
    REQUIRE(TextureChecker::getMinFilter((GLenum)texture->getType()) == (GLenum)texture->getMin());

    REQUIRE(TextureChecker::getMaxAnisotropy((GLenum)texture->getType()) == texture->getAnisotropic());
    REQUIRE(TextureChecker::getSize((GLenum)texture->getType()) == texture->getSize());
    REQUIRE(TextureChecker::getInternalFormat((GLenum)texture->getType()) == (GLenum)texture->getInternalFormat());
    REQUIRE(TextureChecker::isCompressed((GLenum)texture->getType()) == texture->isCompressed());
    REQUIRE(TextureChecker::isImmutable((GLenum)texture->getType()) == texture->isImmutable());
}

enum class ExtensionTextureType {
    State,
    Named,
    BindlessState,
    BindlessNamed
};

enum class TextureType  {
    Mutable,
    Immutable
};

void test_with_type_and_extension_texture(Texture::Type type, ExtensionTextureType ex_type, TextureType tex_type) {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    // build texture without mipmap
    {
        TextureBuilder builder {};

        builder.setTarget(type)
                .setWrapS(Texture::Wrap::ClampToEdge)
                .setWrapT(Texture::Wrap::ClampToEdge)
                .setMipMap(false)
                .setMinFilter(Texture::Filter::Linear)
                .setMagFilter(Texture::Filter::Linear)
                .setInternalFormat(Texture::InternalFormat::RGB8)
                .setFormat(Texture::Format::RGB)
                .setDataType(Texture::DataType::UnsignedByte);

        switch(type) {
            case Texture::Type::Tex2D:
                builder.setSize({512, 512});
                break;
            case Texture::Type::Tex3D:
                builder.setSize({512, 512, 512});
                break;
            case Texture::Type::Tex2DArray:
                builder.setSize({512, 512, 512});
                break;
            default:
                throw std::runtime_error("not testable");
        }

        switch (ex_type) {
            case ExtensionTextureType::State:
                builder.useStateExtensionTexture();
                break;
            case ExtensionTextureType::Named:
                builder.useNamedExtensionTexture();
                break;
            case ExtensionTextureType::BindlessState:
                builder.useStateExtensionTexture();
                builder.useBindlessExtensionTexture();
                break;
            case ExtensionTextureType::BindlessNamed:
                builder.useNamedExtensionTexture();
                builder.useBindlessExtensionTexture();
                break;
        }

        std::shared_ptr<Texture> texture;
        switch (tex_type) {
            case TextureType::Mutable:
                texture = builder.buildMutable();
                break;
            case TextureType::Immutable:
                texture = builder.buildImmutable();
                break;
        }

        check_texture_state(texture);
        check_opengl_state();
    }

    // build texture with mipmap
    {
        TextureBuilder builder {};

        builder.setTarget(type)
                .setWrapS(Texture::Wrap::ClampToEdge)
                .setWrapT(Texture::Wrap::ClampToEdge)
                .setMipMap(true)
                .setMinFilter(Texture::Filter::Linear)
                .setMagFilter(Texture::Filter::Linear)
                .setInternalFormat(Texture::InternalFormat::RGB8)
                .setFormat(Texture::Format::RGB)
                .setDataType(Texture::DataType::UnsignedByte);

        switch(type) {
            case Texture::Type::Tex2D:
                builder.setSize({512, 512});
                break;
            case Texture::Type::Tex3D:
                builder.setSize({512, 512, 512});
                break;
            case Texture::Type::Tex2DArray:
                builder.setSize({512, 512, 512});
                break;
            default:
                throw std::runtime_error("not testable");
        }

        switch (ex_type) {
            case ExtensionTextureType::State:
                builder.useStateExtensionTexture();
                break;
            case ExtensionTextureType::Named:
                builder.useNamedExtensionTexture();
                break;
            case ExtensionTextureType::BindlessState:
                builder.useStateExtensionTexture();
                builder.useBindlessExtensionTexture();
                break;
            case ExtensionTextureType::BindlessNamed:
                builder.useNamedExtensionTexture();
                builder.useBindlessExtensionTexture();
                break;
        }

        std::shared_ptr<Texture> texture;
        switch (tex_type) {
            case TextureType::Mutable:
                texture = builder.buildMutable();
                break;
            case TextureType::Immutable:
                texture = builder.buildImmutable();
                break;
        }

        check_texture_state(texture);
        check_opengl_state();
    }

    // build texture with mipmap and sets properties and invoke functions
    {
        TextureBuilder builder {};

        builder.setTarget(type)
                .setWrapS(Texture::Wrap::ClampToEdge)
                .setWrapT(Texture::Wrap::ClampToEdge)
                .setMipMap(true)
                .setMinFilter(Texture::Filter::Linear)
                .setMagFilter(Texture::Filter::Linear)
                .setInternalFormat(Texture::InternalFormat::RGB8)
                .setFormat(Texture::Format::RGB)
                .setDataType(Texture::DataType::UnsignedByte);

        switch(type) {
            case Texture::Type::Tex2D:
                builder.setSize({512, 512});
                break;
            case Texture::Type::Tex3D:
                builder.setSize({512, 512, 512});
                break;
            case Texture::Type::Tex2DArray:
                builder.setSize({512, 512, 512});
                break;
            default:
                throw std::runtime_error("not testable");
        }

        switch (ex_type) {
            case ExtensionTextureType::State:
                builder.useStateExtensionTexture();
                break;
            case ExtensionTextureType::Named:
                builder.useNamedExtensionTexture();
                break;
            case ExtensionTextureType::BindlessState:
                builder.useStateExtensionTexture();
                builder.useBindlessExtensionTexture();
                break;
            case ExtensionTextureType::BindlessNamed:
                builder.useNamedExtensionTexture();
                builder.useBindlessExtensionTexture();
                break;
        }

        std::shared_ptr<Texture> texture;
        switch (tex_type) {
            case TextureType::Mutable:
                texture = builder.buildMutable();
                break;
            case TextureType::Immutable:
                texture = builder.buildImmutable();
                break;
        }

        check_texture_state(texture);
        check_opengl_state();

        texture->setMinFilter(Texture::Filter::Linear);

        check_texture_state(texture);

        check_opengl_state();

        texture->setMagFilter(Texture::Filter::Linear);

        check_texture_state(texture);

        check_opengl_state();

        texture->setWrapS(Texture::Wrap::Repeat);

        check_texture_state(texture);

        check_opengl_state();

        texture->setWrapT(Texture::Wrap::Repeat);

        check_texture_state(texture);

        check_opengl_state();

        texture->setWrapR(Texture::Wrap::Repeat);

        check_texture_state(texture);

        check_opengl_state();

        texture->setAnisotropicFilter(8.0f);

        check_texture_state(texture);

        check_opengl_state();

        texture->bind(5);

        check_texture_state(texture);

        check_opengl_state();

        switch(type) {
            case Texture::Type::Tex2D:
                texture->resize({512, 512, 1});
                break;
            case Texture::Type::Tex3D:
                texture->resize({512, 512, 512});
                break;
            case Texture::Type::Tex2DArray:
                texture->resize({512, 512, 512});
                break;
            default:
                throw std::runtime_error("not testable");
        }

        check_texture_state(texture);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("[State Mutable Texture 2D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2D, ExtensionTextureType::State, TextureType::Mutable);
}

TEST_CASE("[State Mutable Texture 2D Array]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2DArray, ExtensionTextureType::State, TextureType::Mutable);
}

TEST_CASE("[State Mutable Texture 3D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex3D, ExtensionTextureType::State, TextureType::Mutable);
}

TEST_CASE("[State Immutable Texture 2D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2D, ExtensionTextureType::State, TextureType::Immutable);
}

TEST_CASE("[State Immutable Texture 2D Array]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2DArray, ExtensionTextureType::State, TextureType::Immutable);
}

TEST_CASE("[State Immutable Texture 3D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex3D, ExtensionTextureType::State, TextureType::Immutable);
}

TEST_CASE("[Named Mutable Texture 2D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2D, ExtensionTextureType::Named, TextureType::Mutable);
}

TEST_CASE("[Named Mutable Texture 2D Array]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2DArray, ExtensionTextureType::Named, TextureType::Mutable);
}

TEST_CASE("[Named Mutable Texture 3D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex3D, ExtensionTextureType::Named, TextureType::Mutable);
}

TEST_CASE("[Named Immutable Texture 2D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2D, ExtensionTextureType::Named, TextureType::Immutable);
}

TEST_CASE("[Named Immutable Texture 2D Array]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2DArray, ExtensionTextureType::Named, TextureType::Immutable);
}

TEST_CASE("[Named Immutable Texture 3D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex3D, ExtensionTextureType::Named, TextureType::Immutable);
}

TEST_CASE("[BindlessState Mutable Texture 2D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2D, ExtensionTextureType::BindlessState, TextureType::Mutable);
}

TEST_CASE("[BindlessState Mutable Texture 2D Array]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2DArray, ExtensionTextureType::BindlessState, TextureType::Mutable);
}

TEST_CASE("[BindlessState Mutable Texture 3D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex3D, ExtensionTextureType::BindlessState, TextureType::Mutable);
}

TEST_CASE("[BindlessState Immutable Texture 2D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2D, ExtensionTextureType::BindlessState, TextureType::Immutable);
}

TEST_CASE("[BindlessState Immutable Texture 2D Array]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2DArray, ExtensionTextureType::BindlessState, TextureType::Immutable);
}

TEST_CASE("[BindlessState Immutable Texture 3D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex3D, ExtensionTextureType::BindlessState, TextureType::Immutable);
}

TEST_CASE("[BindlessNamed Mutable Texture 2D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2D, ExtensionTextureType::BindlessNamed, TextureType::Mutable);
}

TEST_CASE("[BindlessNamed Mutable Texture 2D Array]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2DArray, ExtensionTextureType::BindlessNamed, TextureType::Mutable);
}

TEST_CASE("[BindlessNamed Mutable Texture 3D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex3D, ExtensionTextureType::BindlessNamed, TextureType::Mutable);
}

TEST_CASE("[BindlessNamed Immutable Texture 2D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2D, ExtensionTextureType::BindlessNamed, TextureType::Immutable);
}

TEST_CASE("[BindlessNamed Immutable Texture 2D Array]") {
    test_with_type_and_extension_texture(Texture::Type::Tex2DArray, ExtensionTextureType::BindlessNamed, TextureType::Immutable);
}

TEST_CASE("[BindlessNamed Immutable Texture 3D]") {
    test_with_type_and_extension_texture(Texture::Type::Tex3D, ExtensionTextureType::BindlessNamed, TextureType::Immutable);
}
