#include <limitless/core/uniform/uniform_value.hpp>

#include <limitless/core/shader_program.hpp>
#include <limitless/core/bindless_texture.hpp>
#include <limitless/core/context_initializer.hpp>

using namespace Limitless;

template<typename T>
UniformValue<T>::UniformValue(const std::string& name, UniformType type, const T& value)
    : Uniform {name, type, UniformValue::getUniformValueType()}
    , value {std::move(value)} {}

template<typename T>
UniformValue<T>::UniformValue(const std::string& name, const T& value) noexcept
    : Uniform {name, UniformType::Value, UniformValue::getUniformValueType()}
    , value {std::move(value)} { }

template<typename T>
void UniformValue<T>::setValue(const T& val) noexcept {
    if (value != val) {
        value = val;
        changed = true;
    }
}

template<typename T>
void UniformValue<T>::set(const ShaderProgram& shader) {
    const auto location = shader.getUniformLocation(*this);
    if (location == -1) return;

    if constexpr (std::is_same<T, unsigned int>::value) {
        glUniform1ui(location, value);
    }
    else if constexpr (std::is_same<T, int>::value) {
        glUniform1i(location, value);
    }
    else if constexpr (std::is_same<T, float>::value) {
        glUniform1f(location, value);
    }
    else if constexpr (std::is_same<T, glm::vec2>::value) {
        glUniform2fv(location, 1, &value[0]);
    }
    else if constexpr (std::is_same<T, glm::vec3>::value) {
        glUniform3fv(location, 1, &value[0]);
    }
    else if constexpr (std::is_same<T, glm::vec4>::value) {
        glUniform4fv(location, 1, &value[0]);
    }
    else if constexpr (std::is_same<T, glm::mat3>::value) {
        glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
    }
    else if constexpr (std::is_same<T, glm::mat4>::value) {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    } else {
        static_assert(!std::is_same<T, T>::value, "Unimplemented uniform set for type T.");
    }
}

template<typename T>
[[nodiscard]] UniformValue<T>* UniformValue<T>::clone() noexcept {
    return new UniformValue<T>(*this);
}

template<typename T>
constexpr UniformValueType UniformValue<T>::getUniformValueType() {
    if constexpr (std::is_same<T, int>::value) {
        return UniformValueType::Int;
    } else if constexpr (std::is_same<T, unsigned int>::value) {
        return UniformValueType::Uint;
    } else if constexpr (std::is_same<T, float>::value) {
        return UniformValueType::Float;
    } else if constexpr (std::is_same<T, glm::vec2>::value) {
        return UniformValueType::Vec2;
    } else if constexpr (std::is_same<T, glm::vec3>::value) {
        return UniformValueType::Vec3;
    } else if constexpr (std::is_same<T, glm::vec4>::value) {
        return UniformValueType::Vec4;
    } else if constexpr (std::is_same<T, glm::mat3>::value) {
        return UniformValueType::Mat3;
    } else if constexpr (std::is_same<T, glm::mat4>::value) {
        return UniformValueType::Mat4;
    } else {
        static_assert(!std::is_same<T, T>::value, "Unimplemented value type for uniform T.");
    }
}

/**
 * Explicitly instantiating template types
 */
namespace Limitless {
    template class UniformValue<int>;
    template class UniformValue<float>;
    template class UniformValue<unsigned int>;
    template class UniformValue<glm::vec2>;
    template class UniformValue<glm::vec3>;
    template class UniformValue<glm::vec4>;
    template class UniformValue<glm::mat3>;
    template class UniformValue<glm::mat4>;
}
