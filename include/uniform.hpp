#pragma once

#include <texture.hpp>

#include <memory>

namespace GraphicsEngine {
    class ShaderProgram;

    enum class UniformType { Value, Sampler };
    enum class UniformValueType { Float, Int, Uint, Vec2, Vec3, Vec4, Mat4 };

    class Uniform {
    protected:
        std::string name;
        UniformType type;
        UniformValueType value_type;
        bool changed;
    public:
        Uniform(std::string name, UniformType type, UniformValueType value_type) noexcept;
        virtual ~Uniform() = default;

        [[nodiscard]] virtual Uniform* clone() noexcept = 0;

        [[nodiscard]] const auto& getType() const noexcept { return type; }
        [[nodiscard]] const auto& getValueType() const noexcept { return value_type; }
        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] auto& getChanged() noexcept { return changed; }

        virtual void set(const ShaderProgram& shader) = 0;
    };

    template<typename T>
    class UniformValue : public Uniform {
    protected:
        T value;

        constexpr UniformValueType getUniformValueType();
        UniformValue(std::string name, UniformType type, const T& value);
    public:
        UniformValue(std::string name, const T& value) noexcept;
        ~UniformValue() override = default;

        [[nodiscard]] UniformValue* clone() noexcept override;

        [[nodiscard]] const auto& getValue() const noexcept { return value; }

        void set(const ShaderProgram& shader) override;
        void setValue(const T& val) noexcept;
    };

    class UniformSampler : public UniformValue<int> {
    private:
        std::shared_ptr<Texture> sampler;
    public:
        UniformSampler(std::string name, std::shared_ptr<Texture> sampler) noexcept;
        ~UniformSampler() override = default;

        [[nodiscard]] UniformSampler* clone() noexcept override;

        [[nodiscard]] const auto& getSampler() const noexcept { return sampler; }

        void set(const ShaderProgram& shader) override;
        void setSampler(const std::shared_ptr<Texture>& texture);
    };

    std::string getUniformDeclaration(const Uniform& uniform) noexcept;
}