#include <cumstom_material_builder.hpp>
#include <shader_compiler.hpp>
#include <assets.hpp>

using namespace GraphicsEngine;

void CustomMaterialBuilder::clear() noexcept {
    MaterialBuilder::clear();

    uniforms.clear();
    vertex_code.clear();
    fragment_code.clear();
}

void CustomMaterialBuilder::addUniform(std::string name, Uniform* uniform) {
    auto result = uniforms.emplace(std::move(name), uniform);
    if (!result.second) {
        throw std::runtime_error("Failed to add uniform to custom material, already exists.");
    }
}

void CustomMaterialBuilder::setVertexCode(const std::string& vs_code) noexcept {
    vertex_code = vs_code;
}

void CustomMaterialBuilder::setFragmentCode(const std::string& fs_code) noexcept {
    fragment_code = fs_code;
}

std::shared_ptr<Material> CustomMaterialBuilder::build(const std::string &name,
                                                       const GraphicsEngine::RequiredModelShaders& model_shaders,
                                                       const GraphicsEngine::RequiredMaterialShaders& material_shaders)
{
    // checks for empty buffer
    if (properties.empty() || uniforms.empty()) {
        throw std::runtime_error("Properties & Uniforms cannot be empty.");
    }

    material_index = next_shader_index++;

    for (const auto& material_shader : material_shaders) {
        for (const auto& model_shader : model_shaders) {
            ShaderCompiler::compile(*this, material_shader, model_shader);
        }
    }

    getUniformAlignments(shader_storage.get(material_shaders[0], model_shaders[0], material_index));

    auto* custom_material = new CustomMaterial(std::move(properties), std::move(uniform_offsets), blending, shading, name, material_index, std::move(uniforms));
    auto material = std::shared_ptr<CustomMaterial>(custom_material);

    // initializes material buffer
    std::vector<std::byte> data(buffer_size);
    material->material_buffer = BufferBuilder::buildIndexed("material_buffer", Buffer::Type::Uniform, data, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);

    clear();

    assets.materials.add(name, material);

    return material;
}
