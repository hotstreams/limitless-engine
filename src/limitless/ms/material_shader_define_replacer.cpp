#include <limitless/core/context_initializer.hpp>
#include <limitless/ms/material_shader_define_replacer.hpp>

#include <limitless/ms/material.hpp>
#include <limitless/core/shader/shader.hpp>
#include <limitless/core/shader/shader_define_replacer.hpp>
#include <limitless/core/uniform/uniform.hpp>
#include <limitless/core/vertex_stream/vertex_stream.hpp>
#include <limitless/ms/batched_material.hpp>
#include <limitless/renderer/renderer_settings.hpp>

using namespace Limitless::ms;
using namespace Limitless;

VertexStream::InputType getInputTypeFromInstanceType(InstanceType type)
{
    switch (type)
    {
    case InstanceType::Model:
    case InstanceType::Instanced:
        return {
            {0, DataType::Vec3},
            {1, DataType::Vec3},
            {2, DataType::Vec3},
            {3, DataType::Vec2}
        };
    case InstanceType::Skeletal:
        return {
            {0, DataType::Vec3},
            {1, DataType::Vec3},
            {2, DataType::Vec3},
            {3, DataType::Vec2},
            {4, DataType::IVec4},
            {5, DataType::Vec4}
        };
    case InstanceType::BatchedModel:
        break;
    case InstanceType::Effect:
        break;
    case InstanceType::Decal:
        break;
    case InstanceType::Terrain:
        break;
    }
}

std::map<uint8_t, std::string> getNameMappingFromInstanceType(InstanceType type)
{
    switch (type)
    {
    case InstanceType::Model:
    case InstanceType::Instanced:
        return {
            {0, "position"},
            {1, "normal"},
            {2, "tangent"},
            {3, "uv"}
        };
    case InstanceType::Skeletal:
        return {
                {0, "position"},
                {1, "normal"},
                {2, "tangent"},
                {4, "bone_index"},
                {5, "bone_weight"},
        };
    case InstanceType::BatchedModel:
        return {
                    {0, "position"},
                    {1, "normal"},
                    {2, "tangent"},
                    {4, "bone_index"},
                    {5, "bone_weight"},
                    {6, "mesh_index"},
            };
    case InstanceType::Effect:
    case InstanceType::Decal:
    case InstanceType::Terrain:
        break;
    }
}

std::string MaterialShaderDefineReplacer::getPropertyDefines(const Material& material) {
    std::string defines;

    for (const auto& [property, _] : material.getProperties()) {
        auto define = "#define " + PROPERTY_DEFINE.at(property) + '\n';
        defines.append(define);
    }

    if (material.getRefraction()) {
        defines.append("#define ENGINE_MATERIAL_REFRACTION\n");
    }

    if (material.getTwoSided()) {
        defines.append("#define ENGINE_MATERIAL_TWO_SIDED\n");
    }

    return defines;
}

std::string MaterialShaderDefineReplacer::getShadingDefines(const Material& material) {
    return "#define " + SHADING_DEFINE[material.getShading()] + '\n';
}

std::string MaterialShaderDefineReplacer::getModelDefines(InstanceType model_shader) {
    return "#define " + MODEL_DEFINE[model_shader] + '\n';
}

void MaterialShaderDefineReplacer::replaceMaterialDependentDefine(
    Shader &shader,
    const Material &material,
    const RendererSettings& settings,
    InstanceType model_shader) {
    shader.replaceKey(DEFINE_NAMES.at(Define::MaterialDependent), getMaterialDependentDefine(material, model_shader));

    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Vertex], material.getVertexSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Fragment], material.getFragmentSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Global], material.getGlobalSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomScalar], getScalarUniformDefines(material));
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomSamplers], getSamplerUniformDefines(material));
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomShading], material.getShadingSnippet());

    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::Stream], getVertexStreamDeclaration(model_shader) + getVertexStreamGettersDeclaration(model_shader));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::Context],
        getVertexContextDeclaration(model_shader) +
        getVertexContextInterfaceBlockOut(material, settings, model_shader));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::ContextAssignment], getVertexContextCompute(model_shader));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::PassThrough], getVertexPassThrough(material, settings, model_shader));
}

std::string MaterialShaderDefineReplacer::getScalarUniformDefines(const Material &material) {
    std::string uniforms;
    for (const auto& [name, uniform] : material.getUniforms()) {
        if (uniform->getType() != UniformType::Sampler) {
            auto decl = getUniformDeclaration(*uniform);
            decl.erase(decl.find("uniform"), 7);
            uniforms.append(decl);
        }
    }
    return uniforms;
}

std::string MaterialShaderDefineReplacer::getSamplerUniformDefines(const Material &material) {
    std::string uniforms;
    for (const auto& [name, uniform] : material.getUniforms()) {
        if (uniform->getType() == UniformType::Sampler) {
            auto decl = getUniformDeclaration(*uniform);
            decl.erase(decl.find("uniform"), 7);
            uniforms.append(decl);
        }
    }
    return uniforms;
}

std::string MaterialShaderDefineReplacer::getMaterialBufferDeclaration(const Material& material) {
    std::string buffer = "layout (std140) uniform MATERIAL_BUFFER {\n";

    for (const auto& [_, uniform] : material.getProperties()) {
        if (uniform->getType() != UniformType::Sampler && uniform->getType() != UniformType::SamplerArray ||
            (uniform->getType() == UniformType::Sampler || uniform->getType() == UniformType::SamplerArray) && ContextInitializer::isBindlessTextureSupported()) {
            buffer.append(getUniformDeclaration(*uniform));
        }
    }

    for (const auto& [_, uniform] : material.getUniforms()) {
        if (uniform->getType() != UniformType::Sampler && uniform->getType() != UniformType::SamplerArray ||
            (uniform->getType() == UniformType::Sampler || uniform->getType() == UniformType::SamplerArray) && ContextInitializer::isBindlessTextureSupported()) {
            buffer.append(getUniformDeclaration(*uniform));
        }
    }

    buffer.append("uint _material_shading_model;\n");

    buffer.append("};\n");

    for (const auto& [_, uniform] : material.getProperties()) {
        if ((uniform->getType() == UniformType::Sampler || uniform->getType() == UniformType::SamplerArray) && ContextInitializer::isBindlessTextureSupported()) {
            buffer.append(getUniformDeclaration(*uniform));
        }
    }

    for (const auto& [_, uniform] : material.getUniforms()) {
        if ((uniform->getType() == UniformType::Sampler || uniform->getType() == UniformType::SamplerArray) && ContextInitializer::isBindlessTextureSupported()) {
            buffer.append(getUniformDeclaration(*uniform));
        }
    }

    return buffer;
}

// std::string MaterialShaderDefineReplacer::getMaterialGettersDeclaration(const Material& material) {
//     std::string buffer;
//
//     for (const auto& [property, uniform] : material.getProperties()) {
//         switch (uniform->getType()) {
//             case UniformType::Value: {
//                 buffer.append(
//                     getDataTypeString(uniform->getValueType()) + "getMaterial" + getPropertyName(property) + "() {\n"
//                     "\treturn " + getMaterialPropertyBufferName() + ";\n}");
//             }
//             break;
//             case UniformType::Sampler: {
//
//             }
//
//         }
//     }
// }

std::string MaterialShaderDefineReplacer::getMaterialDefines(const Material &material) {
    std::string define = getPropertyDefines(material);
    define.append(getShadingDefines(material));

    if (dynamic_cast<const BatchedMaterial*>(&material)) {
        define.append("#define ENGINE_MATERIAL_BATCHED\n");
    }

    return define;
}

std::string MaterialShaderDefineReplacer::getMaterialDependentDefine(const Material &material, InstanceType model_shader) {
    std::string define = getMaterialDefines(material);
    define.append(getModelDefines(model_shader));
    return define;
}

std::string MaterialShaderDefineReplacer::getVertexStreamDeclaration(InstanceType instance_type) {
    std::string stream_declaration;

    auto input_type = getInputTypeFromInstanceType(instance_type);
    auto name_map = getNameMappingFromInstanceType(instance_type);

    auto generate_layout_line = [] (uint8_t index, const std::string& name, DataType type) {
        return "layout (location = " + std::to_string(index) + ") in " + getDataTypeString(type) + " " + vertex_input_prefix + name + ";\n";
    };

    for (const auto& [index, type] : input_type) {
        stream_declaration += generate_layout_line(index, name_map.at(index), type);
    }

    return stream_declaration;
}

std::string MaterialShaderDefineReplacer::getVertexStreamGettersDeclaration(InstanceType instance_type) {
    std::string getters_declaration;

    auto input_type = getInputTypeFromInstanceType(instance_type);
    auto name_map = getNameMappingFromInstanceType(instance_type);

    auto generate_vertex_getter = [] (uint8_t index, const std::string& name, DataType type) {
        auto upper_name = name;
        upper_name[0] = toupper(upper_name[0]);
        return getDataTypeString(type) + " getVertex" + upper_name + "() {\n return " + vertex_input_prefix + name + ";\n}\n";
    };

    for (const auto& [index, type] : input_type) {
        getters_declaration += generate_vertex_getter(index, name_map.at(index), type);
    }

    return getters_declaration;
}

std::string MaterialShaderDefineReplacer::getVertexContextDeclaration(InstanceType instance_type) {
    std::string context_declaration = "struct VertexContext {\n";

    auto input_type = getInputTypeFromInstanceType(instance_type);
    auto name_map = getNameMappingFromInstanceType(instance_type);

    auto generate_attribute_decl = [] (const std::string& name, DataType type) {
        return getDataTypeString(type) + " " + name + ";\n";
    };

    for (const auto& [index, type] : input_type) {
        context_declaration += generate_attribute_decl(name_map.at(index), type);
    }

    // additional vertex context parameters
    context_declaration += "mat4 model_transform;\n";
    context_declaration += "vec4 world_position;\n";

    context_declaration += "};\n";

    return context_declaration;
}

std::string MaterialShaderDefineReplacer::getVertexContextCompute(InstanceType instance_type) {
    std::string context_assignment;

    auto input_type = getInputTypeFromInstanceType(instance_type);
    auto name_map = getNameMappingFromInstanceType(instance_type);

    auto generate_vertex_getter_name = [] (const std::string& name, DataType type) {
        auto upper_name = name;
        upper_name[0] = toupper(upper_name[0]);
        return " getVertex" + upper_name + "();\n";
    };

    auto get_assignment = [generate_vertex_getter_name] (const std::string& name, DataType type) {
        return "vctx." + name + + " = " + generate_vertex_getter_name(name, type);
    };

    for (const auto& [index, type] : input_type) {
        context_assignment += get_assignment(name_map.at(index), type);
    }

    return context_assignment;
}

std::string MaterialShaderDefineReplacer::getVertexContextInterfaceBlock(
    const Material& material,
    const RendererSettings& settings,
    InstanceType type
) {
    std::string context;

    auto input_type = getInputTypeFromInstanceType(type);
    auto name_map = getNameMappingFromInstanceType(type);

    auto generate_attribute_decl = [] (const std::string& name, DataType type) {
        return getDataTypeString(type) + " " + name + ";\n";
    };

    for (const auto& [index, type] : input_type) {
        context += generate_attribute_decl(name_map.at(index), type);
    }

    // additional vertex context parameters
    context += "vec4 world_position;\n";

    if (settings.normal_mapping && material.getProperties().count(Property::Normal)) {
        context += "mat3 TBN;\n";
    }

    if (type == InstanceType::Instanced) {
        context += "flat int instance_id;\n";
    }

    context += "}";

    return context;
}

std::string MaterialShaderDefineReplacer::getVertexContextInterfaceBlockOut(
    const Material& material,
    const RendererSettings& settings,
    InstanceType type
) {
    std::string context_out = "out _vertex_context {\n";

    context_out += getVertexContextInterfaceBlock(material, settings, type);

    context_out += " _out_vertex_context;\n";

    return context_out;
}

std::string MaterialShaderDefineReplacer::getVertexContextInterfaceBlockIn(
    const Material& material,
    const RendererSettings& settings,
    InstanceType type
) {
    std::string context_out = "in _vertex_context {\n";

    context_out += getVertexContextInterfaceBlock(material, settings, type);

    context_out += " _in_vertex_context;\n";

    return context_out;
}

std::string MaterialShaderDefineReplacer::getVertexPassThrough(
    const Material& material,
    const RendererSettings& settings,
    InstanceType type
)
{
    std::string pass_through;

    auto input_type = getInputTypeFromInstanceType(type);
    auto name_map = getNameMappingFromInstanceType(type);

    auto generate_attribute_decl = [] (const std::string& name, DataType type) {
        return "_out_vertex_context." + name + " = vctx." + name + ";\n";
    };

    for (const auto& [index, type] : input_type) {
        pass_through += generate_attribute_decl(name_map.at(index), type);
    }

    // additional vertex context parameters
    pass_through += "_out_vertex_context.world_position = vctx.world_position;\n";

    if (settings.normal_mapping && material.getProperties().count(Property::Normal)) {
        pass_through += "_out_vertex_context.TBN = vctx.TBN;\n";
    }

    if (type == InstanceType::Instanced) {
        pass_through += "_out_vertex_context.instance_id = gl_InstanceID;\n";
    }

    return pass_through;
}

