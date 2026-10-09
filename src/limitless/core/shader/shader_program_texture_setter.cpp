#include <limitless/core/shader/shader_program_texture_setter.hpp>

#include <limitless/core/texture/texture_extension_capturer.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/texture/texture_binder.hpp>
#include <limitless/core/texture/extension_texture.hpp>
#include <limitless/core/cpu_profiler.hpp>

using namespace Limitless;

void ShaderProgramTextureSetter::bindTextures(const std::map<std::string, std::unique_ptr<Uniform>>& uniforms) {
    CpuProfileScope scope(global_profiler, "ShaderProgramTextureSetter::bindTextures");
    // Collect the sampler uniforms whose textures are bound to texture units (state textures); bindless textures
    // are passed as handles and need no units. Each uniform keeps its own texture: one texture may be sampled by
    // several uniforms (e.g. a glTF material using the same image for base colour and emissive), and every one of
    // them needs its unit set.
    std::vector<UniformSampler*> state_uniforms;
    std::vector<Texture*> state_textures;
    {
        CpuProfileScope scope(global_profiler, "ShaderProgramTextureSetter::bindTextures::collectStateSamplers");
        std::vector<ExtensionTexture*> captured;
        TextureExtensionCapturer capturer {captured};
        for (const auto& [_, uniform] : uniforms) {
            if (uniform->getType() != UniformType::Sampler) {
                continue;
            }
            auto& sampler = static_cast<UniformSampler&>(*uniform); //NOLINT
            const auto captured_before = captured.size();
            sampler.getSampler()->accept(capturer);
            if (captured.size() > captured_before) {
                state_uniforms.emplace_back(&sampler);
                state_textures.emplace_back(sampler.getSampler().get());
            }
        }
    }

    // then we determine to which units these textures are bound and set the units in the uniforms
    const auto units = TextureBinder::bind(state_textures);
    for (size_t i = 0; i < state_uniforms.size(); ++i) {
        state_uniforms[i]->setValue(units[i]);
    }
}
