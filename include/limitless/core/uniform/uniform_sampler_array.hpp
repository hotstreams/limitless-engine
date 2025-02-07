#pragma once

#include <limitless/core/texture/texture.hpp>
#include <limitless/core/uniform/uniform.hpp>

#include "uniform_value_array.hpp"

namespace Limitless {
    class UniformSamplerArray : public UniformValueArray<int32_t> {
    private:
        std::vector<std::shared_ptr<Texture>> samplers;
        std::vector<GLuint> sampler_ids;
    public:
        UniformSamplerArray(std::string name, size_t count) noexcept;
        UniformSamplerArray(std::string name, std::vector<std::shared_ptr<Texture>> samplers) noexcept;
        ~UniformSamplerArray() override = default;

        [[nodiscard]] std::unique_ptr<Uniform> clone() noexcept override;
        void set() override;

        [[nodiscard]] size_t getCount() const noexcept;
        [[nodiscard]] const std::vector<std::shared_ptr<Texture>>& getSamplers() const noexcept;
        void setSampler(size_t index, const std::shared_ptr<Texture>& texture) noexcept;
    };
}
