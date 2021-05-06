#include <limitless/ms/material_builder.hpp>

#include <limitless/core/buffer_builder.hpp>
#include <limitless/ms/material_compiler.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/context_initializer.hpp>

using namespace Limitless::ms;

void MaterialBuilder::createMaterial() {
    // this is private ctor
    material = std::shared_ptr<Material>(new Material());
}

MaterialBuilder::MaterialBuilder(Context& _context, Assets& _assets)
    : context{_context}
    , assets {_assets} {
    createMaterial();
}

void MaterialBuilder::initializeMaterialBuffer() {
    // check the order in material.glsl
    // std140
    // https://www.khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf#page=159

    size_t offset = 0;
    const auto offset_setter = [&] (const auto& container, const std::function<bool(const Uniform&)>& condition = {}) {
        for (const auto& [key, uniform] : container) {
            if (!ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture") && uniform->getType() == UniformType::Sampler) {
                continue;
            }

            if (condition && !condition(*uniform)) {
                continue;
            }

            const auto size = getUniformSize(*uniform);
            const auto alignment = getUniformAlignment(*uniform);

            offset += offset % alignment;

            material->uniform_offsets.emplace(uniform->getName(), offset);
            offset += size;
        }
    };

    offset_setter(material->properties);
    offset_setter(material->uniforms, [] (const Uniform& uniform) { return uniform.getType() == UniformType::Sampler; });
    offset_setter(material->uniforms, [] (const Uniform& uniform) { return uniform.getType() != UniformType::Sampler; });

    BufferBuilder builder;
    material->material_buffer = builder
            .setTarget(Buffer::Type::Uniform)
            .setUsage(Buffer::Usage::DynamicDraw)
            .setAccess(Buffer::MutableAccess::WriteOrphaning)
            .setDataSize(sizeof(std::byte) * offset)
            .build();
}

MaterialBuilder& MaterialBuilder::set(decltype(material->properties)&& properties) {
    //todo check correctness
    material->properties = std::move(properties);
    return *this;
}

MaterialBuilder& MaterialBuilder::set(decltype(material->uniforms)&& uniforms) {
    material->uniforms = std::move(uniforms);
    return *this;
}

MaterialBuilder& MaterialBuilder::add(Property type, float value) {
    switch (type) {
        case Property::Shininess:
            material->properties[type] = std::make_unique<UniformValue<float>>("material_shininess", value);
            break;
        case Property::Metallic:
            material->properties[type] = std::make_unique<UniformValue<float>>("material_metallic", value);
            break;
        case Property::Roughness:
            material->properties[type] = std::make_unique<UniformValue<float>>("material_roughness", value);
            break;
        case Property::TessellationFactor:
        case Property::Color:
        case Property::EmissiveColor:
        case Property::Normal:
        case Property::Displacement:
        case Property::Diffuse:
        case Property::Specular:
        case Property::EmissiveMask:
        case Property::MetallicTexture:
        case Property::RoughnessTexture:
        case Property::BlendMask:
            throw material_builder_error{"Wrong data for material property."};
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::add(Property type, const glm::vec4& value) {
    switch (type) {
        case Property::Color:
            material->properties[type] = std::make_unique<UniformValue<glm::vec4>>("material_color", value);
            break;
        case Property::EmissiveColor:
            material->properties[type] = std::make_unique<UniformValue<glm::vec4>>("material_emissive_color", value);
            break;
        case Property::TessellationFactor:
        case Property::Shininess:
        case Property::Normal:
        case Property::Displacement:
        case Property::Metallic:
        case Property::Roughness:
        case Property::Diffuse:
        case Property::Specular:
        case Property::EmissiveMask:
        case Property::MetallicTexture:
        case Property::RoughnessTexture:
        case Property::BlendMask:
            throw material_builder_error{"Wrong data for material property."};
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::add(Property type, std::shared_ptr<Texture> texture) {
    if (!texture) {
        throw material_builder_error{"Bad texture specified."};
    }

    switch (type) {
        case Property::TessellationFactor:
        case Property::Color:
        case Property::Shininess:
        case Property::Metallic:
        case Property::Roughness:
        case Property::EmissiveColor:
            throw material_builder_error{"Wrong data for material property."};
        case Property::Diffuse:
            material->properties[type] = std::make_unique<UniformSampler>("material_diffuse", std::move(texture));
            break;
        case Property::Specular:
            material->properties[type] = std::make_unique<UniformSampler>("material_specular", std::move(texture));
            break;
        case Property::Normal:
            material->properties[type] = std::make_unique<UniformSampler>("material_normal", std::move(texture));
            break;
        case Property::Displacement:
            material->properties[type] = std::make_unique<UniformSampler>("material_displacement", std::move(texture));
            break;
        case Property::EmissiveMask:
            material->properties[type] = std::make_unique<UniformSampler>("material_emissive_mask", std::move(texture));
            break;
        case Property::BlendMask:
            material->properties[type] = std::make_unique<UniformSampler>("material_blend_mask", std::move(texture));
            break;
        case Property::MetallicTexture:
            material->properties[type] = std::make_unique<UniformSampler>("material_metallic_texture", std::move(texture));
            break;
        case Property::RoughnessTexture:
            material->properties[type] = std::make_unique<UniformSampler>("material_roughness_texture", std::move(texture));
            break;
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::add(Property type, const glm::vec2& value) {
    switch (type) {
        case Property::TessellationFactor:
            material->properties[type] = std::make_unique<UniformValue<glm::vec2>>("material_tessellation", value);
            break;
        case Property::Color:
        case Property::Shininess:
        case Property::Metallic:
        case Property::Roughness:
        case Property::EmissiveColor:
        case Property::Diffuse:
        case Property::Specular:
        case Property::Normal:
        case Property::Displacement:
        case Property::EmissiveMask:
        case Property::BlendMask:
        case Property::MetallicTexture:
        case Property::RoughnessTexture:
            throw material_builder_error{"Wrong data for material property."};
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::remove(Property type) {
    material->properties.erase(type);
    return *this;
}

MaterialBuilder& MaterialBuilder::setBlending(Blending blending) noexcept {
    material->blending = blending;
    return *this;
}

MaterialBuilder& MaterialBuilder::setShading(Shading shading) noexcept {
    material->shading = shading;
    return *this;
}

MaterialBuilder& MaterialBuilder::setTwoSided(bool two_sided) noexcept {
    material->two_sided = two_sided;
    return *this;
}

MaterialBuilder& MaterialBuilder::setName(std::string name) noexcept {
    material->name = std::move(name);
    return *this;
}

MaterialBuilder& MaterialBuilder::setFragmentSnippet(std::string fs_code) noexcept {
    material->fragment_snippet = std::move(fs_code);
    return *this;
}

MaterialBuilder& MaterialBuilder::setVertexSnippet(std::string vs_code) noexcept {
    material->vertex_snippet = std::move(vs_code);
    return *this;
}

MaterialBuilder& MaterialBuilder::setGlobalSnippet(std::string g_code) noexcept {
    material->global_snippet = std::move(g_code);
    return *this;
}

MaterialBuilder& MaterialBuilder::addUniform(std::unique_ptr<Uniform> uniform) {
    material->uniforms[uniform->getName()] = std::move(uniform);
    return *this;
}

MaterialBuilder& MaterialBuilder::removeUniform(const std::string& name) {
    material->uniforms.erase(name);
    return *this;
}

void MaterialBuilder::setMaterialIndex() {
    std::unique_lock lock(mutex);

    if (!material->uniforms.empty() || !material->vertex_snippet.empty() || !material->fragment_snippet.empty() || !material->global_snippet.empty()) {
        // it is custom material; makes index unique
        material->shader_index = next_shader_index++;
    } else {
        const auto material_type = getMaterialType();
        if (auto found = unique_materials.find(material_type); found != unique_materials.end()) {
            // already exist
            material->shader_index = found->second;
        } else {
            // new one
            material->shader_index = next_shader_index++;
            unique_materials.emplace(material_type, material->shader_index);
        }
    }
}

void MaterialBuilder::checkRequirements() {
    if (material->properties.empty()) {
        throw material_builder_error("Properties cannot be empty.");
    }

    if (material->name.empty()) {
        throw material_builder_error("Name cannot be empty.");
    }
}

void MaterialBuilder::compileShaders(const ModelShaders& model_shaders, const PassShaders& pass_shaders) {
    // compiles every required permutation
    // except for ModelShader::Effect
    // because we do not know yet which EffectModules it will use

    MaterialCompiler compiler {context, assets};
    for (const auto& pass_shader : pass_shaders) {
        for (const auto& model_shader : model_shaders) {
            if (model_shader == ModelShader::Effect) {
                continue;
            }

            if (!assets.shaders.contains(pass_shader, model_shader, material->shader_index)) {
                compiler.compile(*material, pass_shader, model_shader);
            }
        }
    }
}

std::shared_ptr<Material> MaterialBuilder::build(const ModelShaders& model_shaders, const PassShaders& pass_shaders) {
    checkRequirements();

    material->model_shaders = model_shaders;

    setMaterialIndex();

    compileShaders(model_shaders, pass_shaders);

    initializeMaterialBuffer();

    auto new_material = material;
    assets.materials.add(material->name, material);

    createMaterial();

    return new_material;
}

UniqueMaterial MaterialBuilder::getMaterialType() const noexcept {
    std::set<Property> props;

    std::for_each(material->properties.begin(), material->properties.end(), [&] (auto& prop) {
        props.emplace(prop.first);
    });

    return { std::move(props), material->shading };
}
