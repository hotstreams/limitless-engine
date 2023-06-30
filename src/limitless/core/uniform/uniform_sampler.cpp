#include <limitless/core/uniform/uniform_sampler.hpp>

#include <limitless/core/texture/texture_uniform_setter.hpp>
#include <limitless/core/texture/texture.hpp>

using namespace Limitless;

UniformSampler::UniformSampler(std::string name, std::shared_ptr<Texture> sampler) noexcept
    : UniformValue {std::move(name), UniformType::Sampler, -1}
    , sampler{std::move(sampler)} {}

std::unique_ptr<Uniform> UniformSampler::clone() noexcept {
    return std::make_unique<UniformSampler>(*this);
}

void UniformSampler::setSampler(const std::shared_ptr<Texture>& texture) noexcept {
    if (sampler != texture || sampler_id != texture->getId()) {
        sampler = texture;
        sampler_id = texture->getId();
        changed = true;
    }
}

const std::shared_ptr<Texture>& UniformSampler::getSampler() const noexcept {
    return sampler;
}

void UniformSampler::set() {
    if (location == -1 || !changed) {
        return;
    }

    // the way its set in shader depends on texture type
    TextureUniformSetter setter {*this};
    sampler->accept(setter);

    changed = false;
}
