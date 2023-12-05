#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"

#include <limitless/core/context.hpp>
#include <limitless/core/texture/state_texture.hpp>

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
};

TEST_CASE("Texture constructor") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateTexture tex = StateTexture();
        tex.generateId();

        REQUIRE(tex.getId() != 0);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Texture activate function") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateTexture tex = StateTexture();

        StateTexture::activate(3);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Texture bind function") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateTexture tex = StateTexture();

        tex.bind(GL_TEXTURE_2D, 3);

        check_opengl_state();
    }

    check_opengl_state();
}
