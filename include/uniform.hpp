#pragma once

#include <texture.hpp>

namespace GraphicsEngine {
    class ShaderProgram;

    enum class UniformType { Value, Sampler };

    class Uniform {
    protected:
        std::string name;
        UniformType type;
        bool changed;
    public:
        Uniform(std::string name, UniformType type) noexcept;
        virtual ~Uniform() = default;

        virtual void set(const ShaderProgram& shader) = 0;
        [[nodiscard]] const auto& getType() const noexcept { return type; }
        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] auto& getChanged() noexcept { return changed; }
    };

    template<typename T>
    class UniformValue : public Uniform {
    protected:
        T value;

        friend ShaderProgram;
        UniformValue(std::string name, UniformType type, const T& value) noexcept;
    public:
        UniformValue(std::string name, const T& value) noexcept;
        ~UniformValue() override = default;

        void set(const ShaderProgram& shader) override;
        void setValue(const T& val) noexcept;
        [[nodiscard]] const auto& getValue() const noexcept { return value; }
    };

    class UniformSampler : public UniformValue<int> {
    private:
        std::shared_ptr<Texture> sampler;
    public:
        UniformSampler(std::string name, std::shared_ptr<Texture> sampler) noexcept;
        ~UniformSampler() override = default;

        void set(const ShaderProgram& shader) override;
        void setSampler(const std::shared_ptr<Texture>& texture);
        [[nodiscard]] const auto& getSampler() const noexcept { return sampler; }
    };
}