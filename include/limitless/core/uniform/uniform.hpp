#pragma once

#include <limitless/core/uniform/uniform_value_type.hpp>
#include <limitless/core/uniform/uniform_type.hpp>
#include <limitless/core/context_debug.hpp>

#include <string>
#include <chrono>
#include <memory>

namespace Limitless {
    class ShaderProgram;

    /*
     * Describes OpenGL Uniform inside shader program that stores some value
     */
    class Uniform {
    protected:
        /*
         *  Unique uniform name inside shader
         */
        std::string name;

        /*
         *  Location inside shader program
         */
        GLint location {-1};

        /*
         *  Uniform type
         */
        UniformType type;

        /*
         * Uniform value type
         */
        UniformValueType value_type;

        /*
         * Stores whether the value was updated since last time
         */
        bool changed {true};

        friend class UniformSerializer;
    public:
        Uniform(std::string name, UniformType type, UniformValueType value_type) noexcept;
        virtual ~Uniform() = default;

        /*
         * Makes uniform deep copy
         */
        [[nodiscard]] virtual std::unique_ptr<Uniform> clone() noexcept = 0;

        /*
         * Sets uniform to current ShaderProgram
         */
        virtual void set() = 0;

        [[nodiscard]] UniformValueType getValueType() const noexcept;
        [[nodiscard]] GLint getLocation() const noexcept;
        [[nodiscard]] const std::string& getName() const noexcept;
        [[nodiscard]] UniformType getType() const noexcept;
        [[nodiscard]] bool isChanged() const noexcept;

        void resetChanged() noexcept;
        void setLocation(GLint location) noexcept;
    };

    bool operator==(const Uniform& lhs, const Uniform& rhs) noexcept;
    bool operator<(const Uniform& lhs, const Uniform& rhs) noexcept;
    bool operator!=(const Uniform& lhs, const Uniform& rhs) noexcept;

    std::string getUniformDeclaration(const Uniform& uniform) noexcept;
    size_t getUniformSize(const Uniform& uniform);
    size_t getUniformAlignment(const Uniform& uniform);
}
