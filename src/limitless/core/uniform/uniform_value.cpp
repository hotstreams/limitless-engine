#include <limitless/core/uniform/uniform_value.hpp>

#include <glm/glm.hpp>

using namespace Limitless;

template<typename T>
constexpr UniformValueType UniformValue<T>::getUniformValueType() {
    if constexpr (std::is_same<T, int32_t>::value) {
        return UniformValueType::Int;
    }
    else if constexpr (std::is_same<T, uint32_t>::value) {
        return UniformValueType::Uint;
    }
    else if constexpr (std::is_same<T, float>::value) {
        return UniformValueType::Float;
    }
    else if constexpr (std::is_same<T, glm::vec2>::value) {
        return UniformValueType::Vec2;
    }
    else if constexpr (std::is_same<T, glm::vec3>::value) {
        return UniformValueType::Vec3;
    }
    else if constexpr (std::is_same<T, glm::vec4>::value) {
        return UniformValueType::Vec4;
    }
    else if constexpr (std::is_same<T, glm::mat3>::value) {
        return UniformValueType::Mat3;
    }
    else if constexpr (std::is_same<T, glm::mat4>::value) {
        return UniformValueType::Mat4;
    }
    else {
        static_assert(!std::is_same<T, T>::value, "Unimplemented value type for uniform T");
    }
}

template<typename T>
UniformValue<T>::UniformValue(std::string name, GLint location, UniformType type, const T& value) noexcept
    : Uniform {std::move(name), location, type, UniformValue::getUniformValueType()}
    , value {value} {}

template<typename T>
UniformValue<T>::UniformValue(std::string name, GLint location, const T& value) noexcept
    : Uniform {std::move(name), location, UniformType::Value, UniformValue::getUniformValueType()}
    , value {value} {}

template<typename T>
std::unique_ptr<Uniform> UniformValue<T>::clone() noexcept {
    return std::make_unique<UniformValue<T>>(*this);
}

template<typename T>
void UniformValue<T>::set() {
    // in case if uniform not present in shader or was not updated since, just return
    // this can happen if shader compiler decided to optimize out our variable
    if (location == -1 || !changed) {
        return;
    }

    if constexpr (std::is_same<T, uint32_t>::value) {
        glUniform1ui(location, value);
    }
    else if constexpr (std::is_same<T, int32_t>::value) {
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
    }
    else {
        static_assert(!std::is_same<T, T>::value, "Unimplemented uniform set for type T.");
    }

    changed = false;
}

template<typename T>
void UniformValue<T>::setValue(const T& val) noexcept {
    if (value != val) {
        value = val;
        changed = true;
    }
}

template<typename T>
const T& UniformValue<T>::getValue() const noexcept {
    return value;
}

namespace Limitless {
    template class UniformValue<int32_t>;
    template class UniformValue<float>;
    template class UniformValue<uint32_t>;
    template class UniformValue<glm::vec2>;
    template class UniformValue<glm::vec3>;
    template class UniformValue<glm::vec4>;
    template class UniformValue<glm::mat3>;
    template class UniformValue<glm::mat4>;
}
