#pragma once

#include <limitless/core/extension_texture.hpp>
#include <limitless/core/texture.hpp>
#include <memory>

namespace Limitless {
    class ImmutableTexture : public Texture {
    private:
        std::unique_ptr<ExtensionTexture> texture;
        glm::vec4 border_color;
        InternalFormat internal_format;
        DataType data_type;
        GLsizei levels {};
        glm::uvec3 size;
        Format format;
        Type target;
        Filter min, mag;
        Wrap wrap_r, wrap_s, wrap_t;
        float anisotropic {};
        bool mipmap;
        bool border;

        void setParameters();
        void texStorage2D(GLenum target, GLsizei levels, InternalFormat internal_format, glm::uvec2 size) const noexcept;
        void texStorage3D(GLenum target, GLsizei levels, InternalFormat internal_format, glm::uvec3 size) const noexcept;
    public:
        // 2D texture construction
        ImmutableTexture(std::unique_ptr<ExtensionTexture> texture,
                         Type target,
                         GLsizei levels,
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
        ImmutableTexture(std::unique_ptr<ExtensionTexture> texture,
                         Type target,
                         GLsizei levels,
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
        ImmutableTexture(std::unique_ptr<ExtensionTexture> texture,
                         Type target,
                         GLsizei levels,
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

        ImmutableTexture(const ImmutableTexture&) = delete;
        ImmutableTexture& operator=(const ImmutableTexture&) = delete;

        ImmutableTexture(ImmutableTexture&&) noexcept = default;
        ImmutableTexture& operator=(ImmutableTexture&&) noexcept = default;

        void texSubImage2D(glm::uvec2 offset, glm::uvec2 size, const void *data) const noexcept override;
        void texSubImage3D(glm::uvec3 offset, glm::uvec3 size, const void *data) const noexcept override;

        void bind(GLuint index) const noexcept override;
        void generateMipMap() noexcept override;
        void resize(glm::uvec3 size) noexcept override;

        ImmutableTexture& setMinFilter(Filter filter) override;
        ImmutableTexture& setMagFilter(Filter filter) override;
        ImmutableTexture& setAnisotropicFilter(float value) override;
        ImmutableTexture& setAnisotropicFilterMax() override;
        ImmutableTexture& setBorderColor(const glm::vec4& color) override;
        ImmutableTexture& setWrapS(Wrap wrap) override;
        ImmutableTexture& setWrapT(Wrap wrap) override;
        ImmutableTexture& setWrapR(Wrap wrap) override;

        [[nodiscard]] GLuint getId() const noexcept override;
        [[nodiscard]] Type getType() const noexcept override;
        [[nodiscard]] glm::uvec3 getSize() const noexcept override;
        [[nodiscard]] ExtensionTexture& getExtensionTexture() noexcept override;

        void accept(TextureVisitor& visitor) noexcept override;
    };
}