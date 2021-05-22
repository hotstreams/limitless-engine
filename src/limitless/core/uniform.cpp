#include <limitless/core/uniform.hpp>

#include <limitless/core/shader_program.hpp>
#include <limitless/core/bindless_texture.hpp>
#include <limitless/core/texture.hpp>
#include <limitless/core/context_initializer.hpp>

using namespace Limitless;

Uniform::Uniform(std::string name, UniformType type, UniformValueType value_type) noexcept
    : name{std::move(name)}
    , type{type}
    , value_type{value_type}
    , changed{true} {}

bool Limitless::operator<(const Uniform& lhs, const Uniform& rhs) noexcept {
    if (lhs.type != rhs.type) {
        return lhs.type < rhs.type;
    }

    if (lhs.value_type != rhs.value_type) {
        return lhs.value_type < rhs.value_type;
    }

    switch (lhs.type) {
        case UniformType::Sampler:
            return static_cast<const UniformSampler&>(lhs).getSampler() < static_cast<const UniformSampler&>(rhs).getSampler();
        case UniformType::Value:
        case UniformType::Time:
            switch (lhs.value_type) {
                case UniformValueType::Float:
                    return static_cast<const UniformValue<float>&>(lhs).getValue() < static_cast<const UniformValue<float>&>(rhs).getValue();
                case UniformValueType::Int:
                    return static_cast<const UniformValue<int>&>(lhs).getValue() < static_cast<const UniformValue<int>&>(rhs).getValue();
                case UniformValueType::Uint:
                    return static_cast<const UniformValue<unsigned int>&>(lhs).getValue() < static_cast<const UniformValue<unsigned int>&>(rhs).getValue();
                case UniformValueType::Vec2: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::vec2>&>(lhs).getValue();
                    const auto& rhs_v = static_cast<const UniformValue<glm::vec2>&>(rhs).getValue();
                    return std::tie(lhs_v.x, lhs_v.y) < std::tie(rhs_v.x, rhs_v.y);
                }
                case UniformValueType::Vec3: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::vec3>&>(lhs).getValue();
                    const auto& rhs_v = static_cast<const UniformValue<glm::vec3>&>(rhs).getValue();
                    return std::tie(lhs_v.x, lhs_v.y, lhs_v.z) < std::tie(rhs_v.x, rhs_v.y, rhs_v.z);
                }
                case UniformValueType::Vec4: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::vec4>&>(lhs).getValue();
                    const auto& rhs_v = static_cast<const UniformValue<glm::vec4>&>(rhs).getValue();
                    return std::tie(lhs_v.x, lhs_v.y, lhs_v.z, lhs_v.w) < std::tie(rhs_v.x, rhs_v.y, rhs_v.z, rhs_v.w);
                }
                case UniformValueType::Mat3:
                case UniformValueType::Mat4:
                    // TODO: for better times
                    return false;
            }

    }
}

bool Limitless::operator==(const Uniform& lhs, const Uniform& rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool Limitless::operator!=(const Uniform& lhs, const Uniform& rhs) noexcept {
    return !(lhs == rhs);
}

template<typename T>
UniformValue<T>::UniformValue(const std::string& name, UniformType type, const T& value)
    : Uniform{name, type, UniformValue::getUniformValueType()}, value{std::move(value)} { }

template<typename T>
UniformValue<T>::UniformValue(const std::string& name, const T& value) noexcept
    : Uniform{name, UniformType::Value, UniformValue::getUniformValueType()}, value{std::move(value)} { }

template<typename T>
void UniformValue<T>::setValue(const T& val) noexcept {
    if (value != val) {
        value = val;
        changed = true;
    }
}

template<typename T>
void UniformValue<T>::set(const ShaderProgram& shader) {
    auto location = shader.getUniformLocation(*this);
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
    }
    else {
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
    }
    else if constexpr (std::is_same<T, unsigned int>::value) {
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
        static_assert(!std::is_same<T, T>::value, "Unimplemented value type for uniform T.");
    }
}

UniformSampler::UniformSampler(const std::string& name, std::shared_ptr<Texture> sampler) noexcept
    : UniformValue{name, UniformType::Sampler, -1}, sampler{std::move(sampler)} { }

void UniformSampler::setSampler(const std::shared_ptr<Texture>& texture) noexcept {
    if (sampler != texture || sampler_id != texture->getId()) {
        sampler = texture;
        sampler_id = texture->getId();
        changed = true;
    }
}

void UniformSampler::set(const ShaderProgram& shader) {
    auto location = shader.getUniformLocation(*this);
    if (location == -1) return;

    // if texture is bindless -> set handle
    // else regular -> set texture unit

    //TODO: remove RTTI
    try {
        auto& texture = dynamic_cast<BindlessTexture&>(sampler->getExtensionTexture());
        texture.makeResident();
        glUniformHandleui64ARB(location, texture.getHandle());
    } catch (...) {
        UniformValue::set(shader);
    }
}

[[nodiscard]] UniformSampler* UniformSampler::clone() noexcept {
    return new UniformSampler(*this);
}

std::string Limitless::getUniformDeclaration(const Uniform& uniform) noexcept {
    std::string declaration = "uniform ";

    switch (uniform.getType()) {
        case UniformType::Time:
        case UniformType::Value:
            switch (uniform.getValueType()) {
                case UniformValueType::Float:
                    declaration.append("float ");
                    break;
                case UniformValueType::Int:
                    declaration.append("int ");
                    break;
                case UniformValueType::Uint:
                    declaration.append("uint ");
                    break;
                case UniformValueType::Vec2:
                    declaration.append("vec2 ");
                    break;
                case UniformValueType::Vec3:
                    declaration.append("vec3 ");
                    break;
                case UniformValueType::Vec4:
                    declaration.append("vec4 ");
                    break;
                case UniformValueType::Mat4:
                    declaration.append("mat4 ");
                    break;
                case UniformValueType::Mat3:
                    declaration.append("mat3 ");
                    break;
            }
            break;
        case UniformType::Sampler:
            const auto& sampler = static_cast<const UniformSampler&>(uniform).getSampler();
            switch (sampler->getType()) {
                case Texture::Type::Tex2D:
                    declaration.append("sampler2D ");
                    break;
                case Texture::Type::Tex3D:
                    declaration.append("sampler3D ");
                    break;
                case Texture::Type::CubeMap:
                    declaration.append("samplerCube ");
                    break;
                case Texture::Type::Tex2DArray:
                    declaration.append("sampler2DArray ");
                    break;
                case Texture::Type::TexCubeMapArray:
                    declaration.append("samplerCubeArray ");
                    break;
//                case Texture::Type::Tex2DMS:
//                    declaration.append("gsampler2DMS ");
//                    break;
            }
            break;
    }

    declaration.append(uniform.getName() + ";\n");

    return declaration;
}

UniformTime::UniformTime(const std::string& name) noexcept
    : UniformValue{name, UniformType::Time, 0.0f} {

}

void UniformTime::update() noexcept {
    if (start == std::chrono::time_point<std::chrono::steady_clock>{}) {
        start = std::chrono::steady_clock::now();
    }

    setValue(std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start).count());
}

void UniformTime::set(const ShaderProgram& shader) {
    update();
    UniformValue::set(shader);
}

UniformTime* UniformTime::clone() noexcept {
    return new UniformTime(*this);
}

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

size_t Limitless::getUniformSize(const Uniform& uniform) {
    switch (uniform.getType()) {
        case UniformType::Value:
            switch (uniform.getValueType()) {
                case UniformValueType::Float:
                    return sizeof(float);
                case UniformValueType::Int:
                    return sizeof(int32_t);
                case UniformValueType::Uint:
                    return sizeof(uint32_t);
                case UniformValueType::Vec2:
                    return sizeof(glm::vec2);
                case UniformValueType::Vec3:
                    return sizeof(glm::vec3);
                case UniformValueType::Vec4:
                    return sizeof(glm::vec4);
                case UniformValueType::Mat3:
                    return sizeof(glm::mat3);
                case UniformValueType::Mat4:
                    return sizeof(glm::mat4);
            }
            break;
        case UniformType::Sampler:
            return ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture") ? sizeof(uint64_t) : 0;
        case UniformType::Time:
            return sizeof(float);
    }
}

size_t Limitless::getUniformAlignment(const Uniform& uniform) {
    switch (uniform.getType()) {
        case UniformType::Value:
            switch (uniform.getValueType()) {
                case UniformValueType::Float:
                    return sizeof(float);
                case UniformValueType::Int:
                    return sizeof(int32_t);
                case UniformValueType::Uint:
                    return sizeof(uint32_t);
                case UniformValueType::Vec2:
                    return sizeof(glm::vec2);
                case UniformValueType::Vec3:
                    return sizeof(glm::vec4); // red zone
                case UniformValueType::Vec4:
                    return sizeof(glm::vec4);
                case UniformValueType::Mat3:
                    return sizeof(glm::vec4); // red zone
                case UniformValueType::Mat4:
                    return sizeof(glm::vec4); // red zone
            }
            break;
        case UniformType::Sampler:
            return ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture") ? sizeof(uint64_t) : 0;
        case UniformType::Time:
            return sizeof(float);
    }
}
