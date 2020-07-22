#include <uniform.hpp>
#include <shader_program.hpp>

using namespace GraphicsEngine;

Uniform::Uniform(std::string name, UniformType type) noexcept : name{std::move(name)}, type{type}, changed{true} {}

template<typename T>
UniformValue<T>::UniformValue(std::string name, UniformType type, const T& value) noexcept
    : Uniform{std::move(name), type}, value{std::move(value)} { }

template<typename T>
UniformValue<T>::UniformValue(std::string name, const T& value) noexcept
    : Uniform{std::move(name), UniformType::Value}, value{std::move(value)} { }

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

namespace GraphicsEngine {
    template class UniformValue<int>;
    template class UniformValue<float>;
    template class UniformValue<unsigned int>;
    template class UniformValue<glm::vec2>;
    template class UniformValue<glm::vec3>;
    template class UniformValue<glm::vec4>;
    template class UniformValue<glm::mat4>;
}
