#include <material_builder.hpp>
#include <shader_compiler.hpp>
#include <assets.hpp>
#include <iostream>

using namespace GraphicsEngine;

void MaterialBuilder::getUniformAlignments(const std::shared_ptr<ShaderProgram>& shader) noexcept {
    const auto found = std::find_if(shader->indexed_binds.begin(), shader->indexed_binds.end(), [&] (const auto& data) { return data.name == "material_buffer"; });
    const auto block_index = found->block_index;

    const std::array<GLenum, 2> block_properties = { GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_DATA_SIZE };
    const GLenum active_variables = GL_ACTIVE_VARIABLES;
    const std::array<GLenum, 2> uniform_prop = { GL_NAME_LENGTH, GL_OFFSET };

    std::array<GLint, 2> block_values = { 0 };
    glGetProgramResourceiv(shader->getId(), GL_UNIFORM_BLOCK, block_index, block_properties.size(), block_properties.data(), block_values.size(), nullptr, block_values.data());

    buffer_size = block_values[1];

    std::vector<GLint> block_uniforms(block_values[0]);
    glGetProgramResourceiv(shader->getId(), GL_UNIFORM_BLOCK, block_index, 1, &active_variables, block_values[0], nullptr, block_uniforms.data());

    for (GLint i = 0; i < block_values[0]; ++i) {
        std::array<GLint, 2> values = { 0 };
        glGetProgramResourceiv(shader->getId(), GL_UNIFORM, block_uniforms[i], uniform_prop.size(), uniform_prop.data(), values.size(), nullptr, values.data());

        std::string name;
        name.resize(values[0] - 1);

        glGetProgramResourceName(shader->getId(), GL_UNIFORM, block_uniforms[i], values[0], nullptr, name.data());

        uniform_offsets.emplace(name, values[1]);
    }
}

MaterialBuilder& MaterialBuilder::add(PropertyType type, float value) {
    switch (type) {
        case PropertyType::Shininess:
            properties.emplace(type, new UniformValue<float>("material_shininess", value));
            break;
        case PropertyType::Metallic:
            properties.emplace(type, new UniformValue<float>("material_metallic", value));
            break;
        case PropertyType::Roughness:
            properties.emplace(type, new UniformValue<float>("material_roughness", value));
            break;
        case PropertyType::Color:
        case PropertyType::EmissiveColor:
        case PropertyType::Normal:
        case PropertyType::Displacement:
        case PropertyType::Diffuse:
        case PropertyType::Specular:
        case PropertyType::EmissiveMask:
        case PropertyType::BlendMask:
            throw std::runtime_error("Wrong data for material property.");
    }
    return *this;
}

MaterialBuilder& MaterialBuilder::add(PropertyType type, const glm::vec4& value) {
    switch (type) {
        case PropertyType::Color:
            properties.emplace(type, new UniformValue<glm::vec4>("material_color", value));
            break;
        case PropertyType::EmissiveColor:
            properties.emplace(type, new UniformValue<glm::vec4>("material_emissive_color", value));
            break;
        case PropertyType::Shininess:
        case PropertyType::Normal:
        case PropertyType::Displacement:
        case PropertyType::Metallic:
        case PropertyType::Roughness:
        case PropertyType::Diffuse:
        case PropertyType::Specular:
        case PropertyType::EmissiveMask:
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
            properties.emplace(type, new UniformSampler("material_diffuse", std::move(texture)));
            break;
        case PropertyType::Specular:
            properties.emplace(type, new UniformSampler("material_specular", std::move(texture)));
            break;
        case PropertyType::Normal:;
            properties.emplace(type, new UniformSampler("material_normal", std::move(texture)));
            break;
        case PropertyType::Displacement:
            properties.emplace(type, new UniformSampler("material_displacement", std::move(texture)));
            break;
        case PropertyType::EmissiveMask:
            properties.emplace(type, new UniformSampler("material_emissive_mask", std::move(texture)));
            break;
        case PropertyType::BlendMask:
            properties.emplace(type, new UniformSampler("material_blend_mask", std::move(texture)));
            break;
    }
    return *this;
}

std::shared_ptr<Material> MaterialBuilder::build(const std::string& name, const RequiredModelShaders& model_shaders, const RequiredMaterialShaders& material_shaders) {
    auto material_type = getMaterialType();

    // sets current unique-material-type index
    auto found = unique_materials.find(material_type);
    material_index = (found != unique_materials.end()) ? found->second : next_shader_index++;
    if (found == unique_materials.end()) {
        unique_materials.emplace(material_type, material_index);
    }

    for (const auto& mat_shader : material_shaders) {
        for (const auto& mod_shader : model_shaders) {
            if(!shader_storage.isExist(mat_shader, mod_shader, material_index)) {
                ShaderCompiler::compile(*this, mat_shader, mod_shader);
            }
        }
    }

    getUniformAlignments(shader_storage.get(material_shaders[0], model_shaders[0], material_index));

    auto* regular_material = new Material(std::move(properties), std::move(uniform_offsets), blending, shading, name, material_index);
    auto material = std::shared_ptr<Material>(regular_material);

    // initializes material buffer
    std::vector<std::byte> data(buffer_size);
    material->material_buffer = BufferBuilder::buildIndexed("material_buffer", Buffer::Type::Uniform, data, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);

    clear();

    assets.materials.add(name, material);

    return material;
}

MaterialType MaterialBuilder::getMaterialType() const noexcept {
    std::vector<PropertyType> props;
    for (const auto& [type, uniform] : properties) {
        props.emplace_back(type);
    }

    return MaterialType{props, blending, shading};
}

MaterialBuilder& MaterialBuilder::setBlending(Blending _blending) noexcept {
    blending = _blending;
    return *this;
}

MaterialBuilder& MaterialBuilder::setShading(Shading _shading) noexcept {
    shading = _shading;
    return *this;
}

void MaterialBuilder::clear() noexcept {
    properties.clear();
    uniform_offsets.clear();
    blending = Blending::Opaque;
    shading = Shading::Unlit;
    material_index = 0;
    buffer_size = 0;
}
