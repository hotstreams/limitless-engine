#include <limitless/material_system/material_builder.hpp>
#include <limitless/core/shader_compiler.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/shader_storage.hpp>
#include <limitless/material_system/material_compiler.hpp>
#include <limitless/core/buffer_builder.hpp>

using namespace LimitlessEngine;

void MaterialBuilder::initializeMaterialBuffer(Material& material, const ShaderProgram& shader) noexcept {
    const auto found = std::find_if(shader.indexed_binds.begin(), shader.indexed_binds.end(), [&] (const auto& data) { return data.name == "material_buffer"; });
    const auto block_index = found->block_index;

    const std::array<GLenum, 2> block_properties = { GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_DATA_SIZE };
    const GLenum active_variables = GL_ACTIVE_VARIABLES;
    const std::array<GLenum, 2> uniform_prop = { GL_NAME_LENGTH, GL_OFFSET };

    std::array<GLint, 2> block_values = { 0 };
    glGetProgramResourceiv(shader.getId(), GL_UNIFORM_BLOCK, block_index, block_properties.size(), block_properties.data(), block_values.size(), nullptr, block_values.data());

    material.material_buffer = BufferBuilder::buildIndexed("material_buffer", Buffer::Type::Uniform, sizeof(std::byte) * block_values[1], Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);

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
            material->properties.emplace(type, new UniformValue<float>("material_shininess", value));
            break;
        case PropertyType::Metallic:
            material->properties.emplace(type, new UniformValue<float>("material_metallic", value));
            break;
        case PropertyType::Roughness:
            material->properties.emplace(type, new UniformValue<float>("material_roughness", value));
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
            throw std::runtime_error("Wrong data for material property.");
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::add(PropertyType type, const glm::vec4& value) {
    switch (type) {
        case PropertyType::Color:
            material->properties.emplace(type, new UniformValue<glm::vec4>("material_color", value));
            break;
        case PropertyType::EmissiveColor:
            material->properties.emplace(type, new UniformValue<glm::vec4>("material_emissive_color", value));
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
            throw std::runtime_error("Wrong data for material property.");
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::add(PropertyType type, std::shared_ptr<Texture> texture) {
    switch (type) {
        case PropertyType::Color:
        case PropertyType::Shininess:
        case PropertyType::Metallic:
        case PropertyType::Roughness:
        case PropertyType::EmissiveColor:
            throw std::runtime_error("Wrong data for material property.");
        case PropertyType::Diffuse:
            material->properties.emplace(type, new UniformSampler("material_diffuse", std::move(texture)));
            break;
        case PropertyType::Specular:
            material->properties.emplace(type, new UniformSampler("material_specular", std::move(texture)));
            break;
        case PropertyType::Normal:
            material->properties.emplace(type, new UniformSampler("material_normal", std::move(texture)));
            break;
        case PropertyType::Displacement:
            material->properties.emplace(type, new UniformSampler("material_displacement", std::move(texture)));
            break;
        case PropertyType::EmissiveMask:
            material->properties.emplace(type, new UniformSampler("material_emissive_mask", std::move(texture)));
            break;
        case PropertyType::BlendMask:
            material->properties.emplace(type, new UniformSampler("material_blend_mask", std::move(texture)));
            break;
        case PropertyType::MetallicTexture:
            material->properties.emplace(type, new UniformSampler("material_metallic_texture", std::move(texture)));
            break;
        case PropertyType::RoughnessTexture:
            material->properties.emplace(type, new UniformSampler("material_roughness_texture", std::move(texture)));
            break;
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::create(std::string name) {
    material = std::unique_ptr<Material>(new Material());
    material->name = std::move(name);
    return *this;
}

std::shared_ptr<Material> MaterialBuilder::build(const ModelShaders& model_shaders, const MaterialShaders& material_shaders) {
    // check for initialized ptr
    if (!material) {
        throw std::runtime_error("Forgot to call create(name)!");
    }
    // checks for empty buffer
    if (material->properties.empty()) {
        throw std::runtime_error("Properties & Uniforms cannot be empty.");
    }

    auto material_type = getMaterialType();

    {
        std::unique_lock lock(mutex);
        // sets current shader index & adds it to storage
        if (auto found = unique_materials.find(material_type); found != unique_materials.end()) {
            material->shader_index = found->second;
        } else {
            material->shader_index = next_shader_index++;
            unique_materials.emplace(material_type, material->shader_index);
        }
    }

    // compiles every material/shader combination
    MaterialCompiler compiler;
    for (const auto& mat_shader : material_shaders) {
        for (const auto& mod_shader : model_shaders) {
            if(!shader_storage.isExist(mat_shader, mod_shader, material->shader_index)) {
                compiler.compile(*material, mat_shader, mod_shader);
            }
        }
    }

    // initializes uniform material buffer using program shader introspection
    initializeMaterialBuffer(*material, *shader_storage.get(material_shaders[0], model_shaders[0], material->shader_index));

    // adds compiled material to assets
    auto compiled = std::shared_ptr<Material>(std::move(material));
    assets.materials.add(compiled->name, compiled);

    return compiled;
}

MaterialType MaterialBuilder::getMaterialType() const noexcept {
    std::vector<PropertyType> props;
    props.reserve(material->properties.size());
    for (const auto& [type, uniform] : material->properties) {
        props.emplace_back(type);
    }

    return MaterialType{std::move(props), material->shading};
}

void MaterialBuilder::setProperties(decltype(Material::properties)&& props) {
    material->properties = std::move(props);
}
