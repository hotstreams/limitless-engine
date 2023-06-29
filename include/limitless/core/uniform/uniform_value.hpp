#pragma once

#include <limitless/core/texture_visitor.hpp>
#include <limitless/core/context_debug.hpp>

#include <limitless/core/uniform/uniform.hpp>

namespace Limitless {
    class Texture;
    class ShaderProgram;

    /**
     * Describes OpenGL uniform value with T type
     */
    template<typename T>
    class UniformValue : public Uniform {
    protected:
        /**
         * Mapped value
         */
        T value;

        /**
         * Gets UniformValueType from template parameter
         */
        constexpr UniformValueType getUniformValueType();

        /**
         * Protected constructor for derived types
         */
        UniformValue(const std::string& name, UniformType type, const T& value);

        friend class UniformSerializer;
    public:
        /**
         * UniformValue constructor
         *
         * creates uniform with specified name and value
         */
        UniformValue(const std::string& name, const T& value) noexcept;

        /**
         * UniformValue destructor
         */
        ~UniformValue() override = default;

        /**
         * Gets current value
         */
        [[nodiscard]] const auto& getValue() const noexcept { return value; }

        /**
         * Updates current value
         */
        void setValue(const T& val) noexcept;

        /**
         * Makes deep uniform copy
         */
        [[nodiscard]] UniformValue* clone() noexcept override;

        /**
         * Sets uniform to ShaderProgram
         */
        void set(const ShaderProgram& shader) override;
    };
}
