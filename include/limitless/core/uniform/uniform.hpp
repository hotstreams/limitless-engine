#pragma once

#include <limitless/core/texture_visitor.hpp>
#include <limitless/core/context_debug.hpp>

#include <memory>
#include <string>
#include <chrono>

#include <limitless/core/uniform/uniform_type.hpp>
#include <limitless/core/uniform/uniform_value_type.hpp>

namespace Limitless {
    class Texture;
    class ShaderProgram;

    /**
     * Describes OpenGL uniform inside shader program
     */
    class Uniform {
    protected:
        /**
         * Unique uniform name
         */
        std::string name;

        /**
         * Uniform type
         */
        UniformType type;

        /**
         * Uniform value type
         *
         * if present
         */
        UniformValueType value_type;

        /**
         * changed flag to know if it should be mapped to GPU
         *
         * to reduce overall mappings
         */
        bool changed;

        friend class UniformSerializer;

        // compares values equality
        friend bool operator==(const Uniform& lhs, const Uniform& rhs) noexcept;
        friend bool operator<(const Uniform& lhs, const Uniform& rhs) noexcept;
        friend bool operator!=(const Uniform& lhs, const Uniform& rhs) noexcept;
    public:
        Uniform(std::string name, UniformType type, UniformValueType value_type) noexcept;
        virtual ~Uniform() = default;

        [[nodiscard]] auto getType() const noexcept { return type; }
        [[nodiscard]] auto getValueType() const noexcept { return value_type; }
        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] virtual bool& getChanged() noexcept { return changed; }

        //TODO:: fix? is it legal
        void setName(std::string _name) { name = std::move(_name); }

        [[nodiscard]] virtual Uniform* clone() noexcept = 0;
        virtual void set(const ShaderProgram& shader) = 0;
    };

    std::string getUniformDeclaration(const Uniform& uniform) noexcept;
    size_t getUniformSize(const Uniform& uniform);
    size_t getUniformAlignment(const Uniform& uniform);

    bool operator==(const Uniform& lhs, const Uniform& rhs) noexcept;
    bool operator<(const Uniform& lhs, const Uniform& rhs) noexcept;
    bool operator!=(const Uniform& lhs, const Uniform& rhs) noexcept;
}
