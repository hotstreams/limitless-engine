#pragma once

#include <limitless/core/uniform/uniform.hpp>

namespace Limitless {
    /*
     * Describes scalar value stored in OpenGL uniform
     *
     * instantiated for:
     *      int32_t
     *      uint32_t
     *      float
     *      glm::vec2
     *      glm::vec3
     *      glm::vec4
     *      glm::mat3
     *      glm::mat4
     */
    template<typename T>
    class UniformValue : public Uniform {
    protected:
        T value;

        constexpr UniformValueType getUniformValueType();

        UniformValue(std::string name, UniformType type, const T& value) noexcept;

        friend class UniformSerializer;
    public:
        UniformValue(std::string name, const T& value) noexcept;
        ~UniformValue() override = default;

        [[nodiscard]] std::unique_ptr<Uniform> clone() noexcept override;
        void set() override;

        [[nodiscard]] const T& getValue() const noexcept;
        [[nodiscard]] T& getValue() noexcept;
        void setValue(const T& val) noexcept;
    };
}
