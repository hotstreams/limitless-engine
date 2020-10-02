#pragma once

#include <texture.hpp>

#include <memory>

namespace GraphicsEngine {
    class BindlessTexture : public Texture {
    private:
        std::shared_ptr<Texture> texture;
        GLuint64 handle;
        bool resident {false};
    public:
        explicit BindlessTexture(std::shared_ptr<Texture> texture);
        ~BindlessTexture() override;

        [[nodiscard]] const auto& getHandle() const noexcept { return handle; }

        void makeResident() noexcept;
        void makeNonresident() noexcept;

        void bind(GLuint index) const noexcept override;
        void resize(glm::uvec3 size) override;

        void texSubImage2D(GLint xoffset, GLint yoffset, glm::uvec2 size, const void* data) const noexcept override;
        void texSubImage3D(GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, const void* data) const noexcept override;

        void generateMipMap() const noexcept override;

        Texture& operator<<(const TexParameter<GLint>& param) noexcept override;
        Texture& operator<<(const TexParameter<GLfloat>& param) noexcept override;
        Texture& operator<<(const TexParameter<GLint*>& param) noexcept override;
        Texture& operator<<(const TexParameter<GLfloat*>& param) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override;
        [[nodiscard]] Type getType() const noexcept override;
        [[nodiscard]] glm::uvec3 getSize() const noexcept override;

        void accept(TextureVisitor& visitor) const noexcept override;
    };
}