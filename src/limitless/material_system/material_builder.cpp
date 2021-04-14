#include <limitless/material_system/material_builder.hpp>

#include <limitless/material_system/material_compiler.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/assets.hpp>

using namespace LimitlessEngine;

namespace {
    constexpr auto MATERIAL_SHADER_BUFFER_NAME = "material_buffer";
}

MaterialBuilder::MaterialBuilder(Context& _context, Assets& _assets) noexcept
    : context{_context}
    , assets {_assets} {
}

void MaterialBuilder::initializeMaterialBuffer(Material& material, const ShaderProgram& shader) noexcept {
    const auto found = std::find_if(shader.indexed_binds.begin(), shader.indexed_binds.end(), [&] (const auto& data) { return data.name == MATERIAL_SHADER_BUFFER_NAME; });
    const auto block_index = found->block_index;

    const std::array<GLenum, 2> block_properties = { GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_DATA_SIZE };
    const GLenum active_variables = GL_ACTIVE_VARIABLES;
    const std::array<GLenum, 2> uniform_prop = { GL_NAME_LENGTH, GL_OFFSET };

    std::array<GLint, 2> block_values = { 0 };
    glGetProgramResourceiv(shader.getId(), GL_UNIFORM_BLOCK, block_index, block_properties.size(), block_properties.data(), block_values.size(), nullptr, block_values.data());

    BufferBuilder builder;
    material.material_buffer = builder  .setTarget(Buffer::Type::Uniform)
                                        .setUsage(Buffer::Usage::DynamicDraw)
                                        .setAccess(Buffer::MutableAccess::WriteOrphaning)
                                        .setDataSize(sizeof(std::byte) * block_values[1])
                                        .build();

    std::vector<GLint> block_uniforms(block_values[0]);
    glGetProgramResourceiv(shader.getId(), GL_UNIFORM_BLOCK, block_index, 1, &active_variables, block_values[0], nullptr, block_uniforms.data());

    // querying variable offsets in uniform buffer used for material properties to map it with byte presentation
    for (GLint i = 0; i < block_values[0]; ++i) {
        std::array<GLint, 2> values = { 0 };
        glGetProgramResourceiv(shader.getId(), GL_UNIFORM, block_uniforms[i], uniform_prop.size(), uniform_prop.data(), values.size(), nullptr, values.data());

        std::string name;
        name.resize(values[0] - 1);

        glGetProgramResourceName(shader.getId(), GL_UNIFORM, block_uniforms[i], values[0], nullptr, name.data());

        material.uniform_offsets.emplace(name, values[1]);
    }
}

MaterialBuilder& MaterialBuilder::setBlending(Blending blending) noexcept {
    material->blending = blending;
    return *this;
}

MaterialBuilder& MaterialBuilder::setShading(Shading shading) noexcept {
    material->shading = shading;
    return *this;
}

MaterialBuilder& MaterialBuilder::add(PropertyType type, float value) {
    switch (type) {
        case PropertyType::Shininess:
            material->properties[type] = std::make_unique<UniformValue<float>>("material_shininess", value);
            break;
        case PropertyType::Metallic:
            material->properties[type] = std::make_unique<UniformValue<float>>("material_metallic", value);
            break;
        case PropertyType::Roughness:
            material->properties[type] = std::make_unique<UniformValue<float>>("material_roughness", value);
            break;
        case PropertyType::Color:
        case PropertyType::EmissiveColor:
        case PropertyType::Normal:
        case PropertyType::Displacement:
        case PropertyType::Diffuse:
        case PropertyType::Specular:
        case PropertyType::EmissiveMask:
        case PropertyType::MetallicTexture:
        case PropertyType::RoughnessTexture:
        case PropertyType::BlendMask:
            throw material_builder_error{"Wrong data for material property."};
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::add(PropertyType type, const glm::vec4& value) {
    switch (type) {
        case PropertyType::Color:
            material->properties[type] = std::make_unique<UniformValue<glm::vec4>>("material_color", value);
            break;
        case PropertyType::EmissiveColor:
            material->properties[type] = std::make_unique<UniformValue<glm::vec4>>("material_emissive_color", value);
            break;
        case PropertyType::Shininess:
        case PropertyType::Normal:
        case PropertyType::Displacement:
        case PropertyType::Metallic:
        case PropertyType::Roughness:
        case PropertyType::Diffuse:
        case PropertyType::Specular:
        case PropertyType::EmissiveMask:
        case PropertyType::MetallicTexture:
        case PropertyType::RoughnessTexture:
        case PropertyType::BlendMask:
            throw material_builder_error{"Wrong data for material property."};
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::add(PropertyType type, std::shared_ptr<Texture> texture) {
    if (!texture) {
        throw material_builder_error{"Bad texture specified."};
    }

    switch (type) {
        case PropertyType::Color:
        case PropertyType::Shininess:
        case PropertyType::Metallic:
        case PropertyType::Roughness:
        case PropertyType::EmissiveColor:
            throw material_builder_error{"Wrong data for material property."};
        case PropertyType::Diffuse:
            material->properties[type] = std::make_unique<UniformSampler>("material_diffuse", std::move(texture));
            break;
        case PropertyType::Specular:
            material->properties[type] = std::make_unique<UniformSampler>("material_specular", std::move(texture));
            break;
        case PropertyType::Normal:
            material->properties[type] = std::make_unique<UniformSampler>("material_normal", std::move(texture));
            break;
        case PropertyType::Displacement:
            material->properties[type] = std::make_unique<UniformSampler>("material_displacement", std::move(texture));
            break;
        case PropertyType::EmissiveMask:
            material->properties[type] = std::make_unique<UniformSampler>("material_emissive_mask", std::move(texture));
            break;
        case PropertyType::BlendMask:
            material->properties[type] = std::make_unique<UniformSampler>("material_blend_mask", std::move(texture));
            break;
        case PropertyType::MetallicTexture:
            material->properties[type] = std::make_unique<UniformSampler>("material_metallic_texture", std::move(texture));
            break;
        case PropertyType::RoughnessTexture:
            material->properties[type] = std::make_unique<UniformSampler>("material_roughness_texture", std::move(texture));
            break;
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::create(std::string name) {
    material = std::shared_ptr<Material>(new Material());
    material->name = std::move(name);
    return *this;
}

void MaterialBuilder::setMaterialIndex() {
    const auto material_type = getMaterialType();

    std::unique_lock lock(mutex);
    // sets current shader index & adds it to storage
    if (auto found = unique_materials.find(material_type); found != unique_materials.end()) {
        material->shader_index = found->second;
    } else {
        material->shader_index = next_shader_index++;
        unique_materials.emplace(material_type, material->shader_index);
    }
}

void MaterialBuilder::checkRequirements() {
    // checks for initialized ptr
    if (!material) {
        throw material_builder_error("Forgot to call create(name)!");
    }

    // checks for empty properties
    if (material->properties.empty()) {
        throw material_builder_error("Properties cannot be empty.");
    }
}

void MaterialBuilder::compileShaders(const ModelShaders& model_shaders, const MaterialShaders& material_shaders) {
    // compiles every required combination
    // except for ModelShader::Effect
    // because we do not know yet which EffectModules it will use

    MaterialCompiler compiler {context, assets};
    for (const auto& material_shader : material_shaders) {
        for (const auto& model_shader : model_shaders) {
            if (model_shader == ModelShader::Effect) {
                continue;
            }

            if (!assets.shaders.contains(material_shader, model_shader, material->shader_index)) {
                compiler.compile(*material, material_shader, model_shader);
            }
        }
    }
}

std::shared_ptr<Material> MaterialBuilder::build(const ModelShaders& model_shaders, const MaterialShaders& material_shaders) {
    checkRequirements();

    material->model_shaders = model_shaders;

    setMaterialIndex();

    compileShaders(model_shaders, material_shaders);

    // initializes uniform material buffer using shader inspection
    // with any compiled shader (EffectShader is not compiled yet)
    if (auto it = std::find_if(model_shaders.begin(), model_shaders.end(), [] (auto& shader) { return shader != ModelShader::Effect; }); it != model_shaders.end()) {
        initializeMaterialBuffer(*material, assets.shaders.get(material_shaders.at(0), *it, material->shader_index));
    }

    assets.materials.add(material->name, material);
    return material;
}

MaterialType MaterialBuilder::getMaterialType() const noexcept {
    std::vector<PropertyType> props;
    props.reserve(material->properties.size());

    std::for_each(material->properties.begin(), material->properties.end(), [&] (auto& prop) { props.emplace_back(prop.first); });

    return { std::move(props), material->shading };
}

void MaterialBuilder::setProperties(decltype(Material::properties)&& props) {
    material->properties = std::move(props);
}

MaterialBuilder& MaterialBuilder::setTwoSided(bool two_sided) noexcept {
    material->two_sided = two_sided;
    return *this;
}

MaterialBuilder& MaterialBuilder::remove(PropertyType type) {
    material->properties.erase(type);
    return *this;
}
