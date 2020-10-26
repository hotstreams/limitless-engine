#pragma once

#include <memory>
#include <string>
#include <core/texture_visitor.hpp>

namespace GraphicsEngine {
    class Texture;
    class ShaderProgram;

    enum class UniformType { Value, Sampler };
    enum class UniformValueType { Float, Int, Uint, Vec2, Vec3, Vec4, Mat3, Mat4 };

    class Uniform {
    protected:
        std::string name;
        UniformType type;
        UniformValueType value_type;
        bool changed;
    public:
        Uniform(std::string name, UniformType type, UniformValueType value_type) noexcept;
        virtual ~Uniform() = default;

        [[nodiscard]] auto getType() const noexcept { return type; }
        [[nodiscard]] auto getValueType() const noexcept { return value_type; }
        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] auto& getChanged() noexcept { return changed; }

        [[nodiscard]] virtual Uniform* clone() noexcept = 0;
        virtual void set(const ShaderProgram& shader) = 0;
    };

    template<typename T>
    class UniformValue : public Uniform {
    protected:
        T value;

        constexpr UniformValueType getUniformValueType();
        UniformValue(const std::string& name, UniformType type, const T& value);
    public:
        UniformValue(const std::string& name, const T& value) noexcept;
        ~UniformValue() override = default;

        [[nodiscard]] const auto& getValue() const noexcept { return value; }
        void setValue(const T& val) noexcept;

        [[nodiscard]] UniformValue* clone() noexcept override;
        void set(const ShaderProgram& shader) override;
    };

    class UniformSampler : public UniformValue<int> {
    private:
        std::shared_ptr<Texture> sampler;
    public:
        UniformSampler(const std::string& name, std::shared_ptr<Texture> sampler) noexcept;
        ~UniformSampler() override = default;

        [[nodiscard]] const auto& getSampler() const noexcept { return sampler; }
        void setSampler(const std::shared_ptr<Texture>& texture) noexcept;

        [[nodiscard]] UniformSampler* clone() noexcept override;
        void set(const ShaderProgram& shader) override;
    };

    std::string getUniformDeclaration(const Uniform& uniform) noexcept;
}
