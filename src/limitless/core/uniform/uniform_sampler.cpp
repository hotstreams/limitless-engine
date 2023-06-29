#include <limitless/core/uniform/uniform_sampler.hpp>

#include <limitless/core/shader_program.hpp>
#include <limitless/core/bindless_texture.hpp>
#include <limitless/core/texture.hpp>
#include <limitless/core/context_initializer.hpp>

using namespace Limitless;

UniformSampler::UniformSampler(const std::string& name, std::shared_ptr<Texture> sampler) noexcept
    : UniformValue {name, UniformType::Sampler, -1}
    , sampler {std::move(sampler)} { }

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
