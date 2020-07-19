#pragma once

#include <texture.hpp>

namespace GraphicsEngine {
    class BindlessTexture : public Texture {
    private:
        std::unique_ptr<Texture> texture;
        GLuint64 handle;
        bool resident {false};
    public:
        explicit BindlessTexture(std::unique_ptr<Texture> texture);
        ~BindlessTexture() override;

        [[nodiscard]] const auto& getHandle() { return handle; }

        void makeResident() noexcept;
        void makeNonresident() noexcept;

        void bind(GLuint index) const noexcept override;

        void texSubImage2D(GLint xoffset, GLint yoffset, glm::uvec2 size, const void* data) const noexcept override;
        void texSubImage3D(GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, const void* data) const noexcept override;

        void generateMipMap() const noexcept override;

        Texture& operator<<(const TexParameter<GLint>& param) noexcept override;
        Texture& operator<<(const TexParameter<GLfloat>& param) noexcept override;
        Texture& operator<<(const TexParameter<GLint*>& param) noexcept override;
        Texture& operator<<(const TexParameter<GLfloat*>& param) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override;
    };
}