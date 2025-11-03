#pragma once

#include <limitless/core/uniform/uniform.hpp>
#include <vector>

namespace Limitless {
    /*
     * Describes array of scalar values stored in OpenGL uniform
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
    class UniformValueArray : public Uniform {
    protected:
        std::vector<T> values;
        size_t count;

        constexpr UniformValueType getUniformValueType();

        friend class UniformSerializer;
    public:
        UniformValueArray(std::string name, size_t count) noexcept;
        UniformValueArray(std::string name, const std::vector<T>& values) noexcept;
        ~UniformValueArray() override = default;

        [[nodiscard]] std::unique_ptr<Uniform> clone() noexcept override;
        void set() override;

        [[nodiscard]] size_t getCount() const noexcept;
        [[nodiscard]] const std::vector<T>& getValues() const noexcept;
        [[nodiscard]] std::vector<T>& getValues() noexcept;
        void setValue(size_t index, const T& val) noexcept;
    };
}