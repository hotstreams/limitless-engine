#include <limitless/core/shader/shader_program_texture_setter.hpp>

#include <limitless/core/texture/texture_extension_capturer.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/texture_binder.hpp>
#include <limitless/core/extension_texture.hpp>

using namespace Limitless;

void ShaderProgramTextureSetter::bindTextures(const std::map<std::string, std::unique_ptr<Uniform>>& uniforms) {
    // first we collect all passed Texture Samplers
    std::vector<Texture*> samplers;
    for (const auto& [_, uniform] : uniforms) {
        if (uniform->getType() == UniformType::Sampler) {
            samplers.emplace_back(static_cast<UniformSampler&>(*uniform).getSampler().get()); //NOLINT
        }
    }

    // then we collect only state textures
    // because not state (bindless) textures do not need this set up
    std::vector<ExtensionTexture*> state_samplers;
    TextureExtensionCapturer capturer {state_samplers};
    for (const auto& texture : samplers) {
        texture->accept(capturer);
    }

    // then we determine to which units should we bind these textures
    const auto units = TextureBinder::bind(state_samplers);

    // then we update unit values in uniforms and set them in shader
    uint32_t i = 0;
    for (const auto& [name, uniform] : uniforms) {
        if (uniform->getType() == UniformType::Sampler) {
            auto &sampler = static_cast<UniformSampler&>(*uniform); //NOLINT
            if (sampler.getSampler()->getId() == state_samplers[i]->getId()) {
                sampler.setValue(units[i++]);
            }
        }
    }
}
