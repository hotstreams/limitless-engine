#pragma once

#include <limitless/core/texture.hpp>
#include <limitless/core/extension_texture.hpp>
#include <memory>

namespace Limitless {
    class MutableTexture : public Texture {
    private:
        std::unique_ptr<ExtensionTexture> texture;
        glm::vec4 border_color;
        InternalFormat internal_format;
        DataType data_type;
        glm::uvec3 size;
        Format format;
        Type target;
        Filter min, mag;
        Wrap wrap_r, wrap_s, wrap_t;
        float anisotropic {};
        bool mipmap;
        bool border;

        void setParameters();
        void texImage2D(GLenum target, GLsizei levels, InternalFormat internal_format, Format format, DataType type, glm::uvec2 size, const void* data) const noexcept;
        void texImage3D(GLenum target, GLsizei levels, InternalFormat internal_format, Format format, DataType type, glm::uvec3 size, const void *data) const noexcept;
    public:
        // 2D texture construction
        MutableTexture(std::unique_ptr<ExtensionTexture> texture,
                       Type target,
                       InternalFormat internal_format,
                       glm::uvec2 size,
                       Format format,
                       DataType data_type,
                       bool border,
                       bool mipmap,
                       const glm::vec4& border_color,
                       const void* data,
                       Filter min,
                       Filter mag,
                       Wrap s,
                       Wrap t,
                       Wrap r) noexcept;

        // 3D texture / 2D texture array / empty cubemap array construction
        MutableTexture(std::unique_ptr<ExtensionTexture> texture,
                       Type target,
                       InternalFormat internal_format,
                       glm::uvec3 size,
                       Format format,
                       DataType data_type,
                       bool border,
                       bool mipmap,
                       const glm::vec4& border_color,
                       const void* data,
                       Filter min,
                       Filter mag,
                       Wrap s,
                       Wrap t,
                       Wrap r) noexcept;

        // cubemap construction
        MutableTexture(std::unique_ptr<ExtensionTexture> texture,
                       Type target,
                       InternalFormat internal_format,
                       glm::uvec2 size,
                       Format format,
                       DataType data_type,
                       bool border,
                       bool mipmap,
                       const glm::vec4& border_color,
                       const std::array<void*, 6>& data,
                       Filter min,
                       Filter mag,
                       Wrap s,
                       Wrap t,
                       Wrap r) noexcept;

        void texSubImage2D(glm::uvec2 offset, glm::uvec2 size, const void *data) const noexcept override;
        void texSubImage3D(glm::uvec3 offset, glm::uvec3 size, const void *data) const noexcept override;

        void bind(GLuint index) const noexcept override;
        void generateMipMap() noexcept override;
        void resize(glm::uvec3 size) noexcept override;

        MutableTexture& setMinFilter(Filter filter) override;
        MutableTexture& setMagFilter(Filter filter) override;
        MutableTexture& setAnisotropicFilter(float value) override;
        MutableTexture& setAnisotropicFilterMax() override;
        MutableTexture& setBorderColor(const glm::vec4& color) override;
        MutableTexture& setWrapS(Wrap wrap) override;
        MutableTexture& setWrapT(Wrap wrap) override;
        MutableTexture& setWrapR(Wrap wrap) override;

        [[nodiscard]] GLuint getId() const noexcept override;
        [[nodiscard]] Type getType() const noexcept override;
        [[nodiscard]] glm::uvec3 getSize() const noexcept override;
        [[nodiscard]] ExtensionTexture& getExtensionTexture() noexcept override;

        void accept(TextureVisitor& visitor) noexcept override;
    };
}