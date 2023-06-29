#pragma once

#include <limitless/core/texture_visitor.hpp>
#include <limitless/core/context_debug.hpp>

#include <memory>
#include <string>
#include <chrono>

#include <limitless/core/uniform/uniform_value.hpp>

namespace Limitless {
    class Texture;
    class ShaderProgram;

    class UniformSampler : public UniformValue<int> {
    private:
        std::shared_ptr<Texture> sampler;
        GLuint sampler_id{};
        friend class UniformSerializer;
    public:
        UniformSampler(const std::string& name, std::shared_ptr<Texture> sampler) noexcept;
        ~UniformSampler() override = default;

        [[nodiscard]] const auto& getSampler() const noexcept { return sampler; }
        void setSampler(const std::shared_ptr<Texture>& texture) noexcept;

        [[nodiscard]] UniformSampler* clone() noexcept override;
        void set(const ShaderProgram& shader) override;
    };
}
