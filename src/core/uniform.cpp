#include <core/uniform.hpp>

#include <core/shader_program.hpp>
#include <core/bindless_texture.hpp>

using namespace GraphicsEngine;

Uniform::Uniform(std::string name, UniformType type, UniformValueType value_type) noexcept
    : name{std::move(name)}, type{type}, value_type{value_type}, changed{true} {}

template<typename T>
UniformValue<T>::UniformValue(const std::string& name, UniformType type, const T& value)
    : Uniform{name, type, getUniformValueType()}, value{std::move(value)} { }

template<typename T>
UniformValue<T>::UniformValue(const std::string& name, const T& value) noexcept
    : Uniform{name, UniformType::Value, getUniformValueType()}, value{std::move(value)} { }

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

    if constexpr (std::is_same<T, int>::value) {
        glUniform1i(location, value);
    }
    else if constexpr (std::is_same<T, unsigned int>::value) {
        glUniform1ui(location, value);
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
    if (sampler != texture) {
        sampler = texture;
        changed = true;
    }
}

void UniformSampler::set(const ShaderProgram& shader) {
    auto location = shader.getUniformLocation(*this);
    if (location == -1) return;

    // if texture is bindless -> set handle
    // else regular -> set texture unit

    //TODO: remove RTTI
    if (const auto* texture = dynamic_cast<BindlessTexture*>(sampler.get()); texture) {
        glUniformHandleui64ARB(location, texture->getHandle());
    } else {
        UniformValue::set(shader);
    }
}

[[nodiscard]] UniformSampler* UniformSampler::clone() noexcept {
    return new UniformSampler(*this);
}

std::string GraphicsEngine::getUniformDeclaration(const Uniform& uniform) noexcept {
    std::string declaration = "uniform ";

    switch (uniform.getType()) {
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
                case Texture::Type::Tex2DMS:
                    declaration.append("gsampler2DMS ");
                    break;
            }
            break;
    }

    declaration.append(uniform.getName() + ";\n");

    return declaration;
}

namespace GraphicsEngine {
    template class UniformValue<int>;
    template class UniformValue<float>;
    template class UniformValue<unsigned int>;
    template class UniformValue<glm::vec2>;
    template class UniformValue<glm::vec3>;
    template class UniformValue<glm::vec4>;
    template class UniformValue<glm::mat3>;
    template class UniformValue<glm::mat4>;
}
