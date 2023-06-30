#pragma once

#include <limitless/core/uniform/uniform_value.hpp>

namespace Limitless {
    class Texture;

    /*
     *  Describes OpenGL sampler used in shader program
     */
    class UniformSampler : public UniformValue<int32_t> {
    private:
        std::shared_ptr<Texture> sampler;
        GLuint sampler_id {};
        friend class UniformSerializer;
    public:
        UniformSampler(std::string name, GLint location, std::shared_ptr<Texture> sampler) noexcept;
        ~UniformSampler() override = default;

        [[nodiscard]] std::unique_ptr<Uniform> clone() noexcept override;
        void set() override;

        [[nodiscard]] const std::shared_ptr<Texture>& getSampler() const noexcept;
        void setSampler(const std::shared_ptr<Texture>& texture) noexcept;
    };
}
