#include <uniform.hpp>
#include <shader_program.hpp>

using namespace GraphicsEngine;

Uniform::Uniform(std::string name, UniformType type, UniformValueType value_type) noexcept
    : name{std::move(name)}, type{type}, value_type{value_type}, changed{true} {}

template<typename T>
UniformValue<T>::UniformValue(std::string name, UniformType type, const T& value)
    : Uniform{std::move(name), type, getUniformValueType()}, value{std::move(value)} { }

template<typename T>
UniformValue<T>::UniformValue(std::string name, const T& value) noexcept
    : Uniform{std::move(name), UniformType::Value, getUniformValueType()}, value{std::move(value)} { }

template<typename T>
void UniformValue<T>::setValue(const T& val) noexcept {
    if (value != val) {
        value = val;
        changed = true;
    }
}

template<typename T>
void UniformValue<T>::set(const ShaderProgram& shader) {
    if (changed) {
        shader.setUniform(*this);
        changed = false;
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
    } else if constexpr (std::is_same<T, glm::mat4>::value) {
        return UniformValueType::Mat4;
    } else if constexpr (!std::is_same<T, T>::value) {
        throw std::runtime_error("Wrong typename T for UniformValue.");
    }
}

UniformSampler::UniformSampler(std::string name, std::shared_ptr<Texture> sampler) noexcept
    : UniformValue{std::move(name), UniformType::Sampler, -1}, sampler{std::move(sampler)} { }

void UniformSampler::setSampler(const std::shared_ptr<Texture>& texture) {
    if (sampler != texture) {
        sampler = texture;
        changed = true;
    }
}

void UniformSampler::set(const ShaderProgram &shader) {
    if (changed) {
        shader.setUniform(*this);
        changed = false;
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
    template class UniformValue<glm::mat4>;
}
