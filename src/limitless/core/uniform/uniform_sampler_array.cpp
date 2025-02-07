#include <limitless/core/uniform/uniform_value_array.hpp>

#include <glm/glm.hpp>

using namespace Limitless;

template<typename T>
constexpr UniformValueType UniformValueArray<T>::getUniformValueType() {
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
    else if constexpr (std::is_same<T, glm::ivec4>::value) {
        return UniformValueType::IVec4;
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
UniformValueArray<T>::UniformValueArray(std::string name, size_t count) noexcept
    : Uniform {std::move(name), UniformType::ValueArray, UniformValueArray::getUniformValueType()}
    , count {count} {
    values.resize(count);
}

template<typename T>
UniformValueArray<T>::UniformValueArray(std::string name, const std::vector<T>& values) noexcept
    : Uniform {std::move(name), UniformType::ValueArray, UniformValueArray::getUniformValueType()}
    , values {values}
    , count {values.size()} {
}

template<typename T>
std::unique_ptr<Uniform> UniformValueArray<T>::clone() noexcept {
    return std::make_unique<UniformValueArray<T>>(*this);
}

template<typename T>
void UniformValueArray<T>::set() {
    // in case if uniform not present in shader or was not updated since, just return
    // this can happen if shader compiler decided to optimize out our variable
    if (location == -1 || !changed) {
        return;
    }

    if constexpr (std::is_same<T, uint32_t>::value) {
        glUniform1uiv(location, count, values.data());
    }
    else if constexpr (std::is_same<T, int32_t>::value) {
        glUniform1iv(location, count, values.data());
    }
    else if constexpr (std::is_same<T, float>::value) {
        glUniform1fv(location, count, values.data());
    }
    else if constexpr (std::is_same<T, glm::vec2>::value) {
        glUniform2fv(location, count, &values[0][0]);
    }
    else if constexpr (std::is_same<T, glm::vec3>::value) {
        glUniform3fv(location, count, &values[0][0]);
    }
    else if constexpr (std::is_same<T, glm::vec4>::value) {
        glUniform4fv(location, count, &values[0][0]);
    }
    else if constexpr (std::is_same<T, glm::ivec4>::value) {
        glUniform4iv(location, count, &values[0][0]);
    }
    else if constexpr (std::is_same<T, glm::mat3>::value) {
        glUniformMatrix3fv(location, count, GL_FALSE, &values[0][0][0]);
    }
    else if constexpr (std::is_same<T, glm::mat4>::value) {
        glUniformMatrix4fv(location, count, GL_FALSE, &values[0][0][0]);
    }
    else {
        static_assert(!std::is_same<T, T>::value, "Unimplemented uniform set for type T.");
    }

    changed = false;
}

template<typename T>
void UniformValueArray<T>::setValue(size_t index, const T& val) noexcept {
    if (values[index] != val) {
        values[index] = val;
        changed = true;
    }
}

template<typename T>
const std::vector<T>& UniformValueArray<T>::getValues() const noexcept {
    return values;
}

template<typename T>
size_t UniformValueArray<T>::getCount() const noexcept {
    return count;
}

template<typename T>
std::vector<T>& UniformValueArray<T>::getValues() noexcept {
    changed = true;
    return values;
}

namespace Limitless {
    template class UniformValueArray<int32_t>;
    template class UniformValueArray<float>;
    template class UniformValueArray<uint32_t>;
    template class UniformValueArray<glm::vec2>;
    template class UniformValueArray<glm::vec3>;
    template class UniformValueArray<glm::vec4>;
    template class UniformValueArray<glm::ivec4>;
    template class UniformValueArray<glm::mat3>;
    template class UniformValueArray<glm::mat4>;
}
