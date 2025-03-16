#include <iostream>
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
    case InstanceType::Decal:
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
    case InstanceType::Effect:
        return {
            {0, DataType::Vec4},
            {1, DataType::Vec4},
            {2, DataType::Vec4},
            {3, DataType::Vec4},
            {4, DataType::Vec4},
            {5, DataType::Vec4},
            {6, DataType::Vec4}
        };
    case InstanceType::BatchedModel:
    case InstanceType::Terrain:
    default:
        throw std::runtime_error("not implemented");
    }
}

std::map<uint8_t, std::string> getNameMappingFromInstanceType(InstanceType type)
{
    switch (type)
    {
    case InstanceType::Model:
    case InstanceType::Instanced:
    case InstanceType::Decal:
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
                {3, "uv"},
                {4, "bone_index"},
                {5, "bone_weight"},
        };
    case InstanceType::BatchedModel:
        return {
                {0, "position"},
                {1, "normal"},
                {2, "tangent"},
                {3, "uv"},
                {4, "bone_index"},
                {5, "bone_weight"},
                {6, "mesh_index"},
            };
    case InstanceType::Effect:
        return {
            {0, "color"},
            {1, "subUV"},
            {2, "properties"},
            {3, "acceleration_lifetime"},
            {4, "position_size"},
            {5, "rotation_time"},
            {6, "velocity"}
        };
    case InstanceType::Terrain:
        throw std::runtime_error("not implemented");
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
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::VertexContext], getVertexContextDeclaration(material, settings, model_shader));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::InterfaceBlockOut], getVertexContextInterfaceBlockOut(material, settings, model_shader));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::ContextAssignment], getVertexContextCompute(material, settings, model_shader));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::PassThrough], getVertexPassThrough(material, settings, model_shader));

    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::InterfaceBlockIn], getVertexContextInterfaceBlockIn(material, settings, model_shader) + getVertexContextInterfaceBlockInGetters(material, settings, model_shader));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::FragmentContext], getFragmentContextDeclaration(material, settings, model_shader) + getFragmentVertexContextCompute(material, settings, model_shader));
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
try
{
    auto input_type = getInputTypeFromInstanceType(instance_type);
    auto name_map = getNameMappingFromInstanceType(instance_type);

    auto generate_layout_line = [] (uint8_t index, const std::string& name, DataType type) {
        return "layout (location = " + std::to_string(index) + ") in " + getDataTypeString(type) + " " + vertex_input_prefix + name + ";\n";
    };

    for (const auto& [index, type] : input_type) {
        stream_declaration += generate_layout_line(index, name_map.at(index), type);
    }
} catch (const std::exception& e)
{
    std::cout << "test" << std::endl;
}

    return stream_declaration;
}

std::string MaterialShaderDefineReplacer::getVertexStreamGettersDeclaration(InstanceType instance_type) {
    std::string getters_declaration;

    auto input_type = getInputTypeFromInstanceType(instance_type);
    auto name_map = getNameMappingFromInstanceType(instance_type);

    auto to_camel_case = [] (const std::string& input)
    {
        std::string result;
        bool capitalizeNext = true;

        for (char ch : input) {
            if (ch == '_') {
                capitalizeNext = true;
            } else {
                if (capitalizeNext) {
                    result += std::toupper(ch);
                    capitalizeNext = false;
                } else {
                    result += ch;
                }
            }
        }

        if (!result.empty()) {
            result[0] = std::toupper(result[0]);
        }

        return result;
    };

    auto generate_vertex_getter = [to_camel_case] (const std::string& name, DataType type) {
        return getDataTypeString(type) + " getVertex" + to_camel_case(name) + "() {\n return " + vertex_input_prefix + name + ";\n}\n";
    };

    for (const auto& [index, type] : input_type) {
        getters_declaration += generate_vertex_getter(name_map.at(index), type);
    }

    return getters_declaration;
}

std::string MaterialShaderDefineReplacer::getVertexContextDeclaration(
    const Material& material,
    const RendererSettings& settings,
    InstanceType type
) {
    std::string context_declaration = "struct VertexContext {\n";

    auto input_type = getInputTypeFromInstanceType(type);
    auto name_map = getNameMappingFromInstanceType(type);

    auto generate_attribute_decl = [] (const std::string& name, DataType type) {
        return getDataTypeString(type) + " " + name + ";\n";
    };

    for (const auto& [index, type] : input_type) {
        context_declaration += generate_attribute_decl(name_map.at(index), type);
    }

    if (type == InstanceType::Instanced) {
        context_declaration += " int instance_id;\n";
    }

    context_declaration += "};\n";

    return context_declaration;
}

std::string MaterialShaderDefineReplacer::getVertexContextCompute(
    const Material& material,
    const RendererSettings& settings,
    InstanceType type
) {
    std::string context_assignment;

    auto input_type = getInputTypeFromInstanceType(type);
    auto name_map = getNameMappingFromInstanceType(type);

    auto to_camel_case = [] (const std::string& input)
    {
        std::string result;
        bool capitalizeNext = true;

        for (char ch : input) {
            if (ch == '_') {
                capitalizeNext = true;
            } else {
                if (capitalizeNext) {
                    result += std::toupper(ch);
                    capitalizeNext = false;
                } else {
                    result += ch;
                }
            }
        }

        if (!result.empty()) {
            result[0] = std::toupper(result[0]);
        }

        return result;
    };

    auto generate_vertex_getter = [to_camel_case] (const std::string& name, DataType type) {
        return "getVertex" + to_camel_case(name) + "();\n";
    };

    auto get_assignment = [generate_vertex_getter] (const std::string& name, DataType type) {
        return "vctx." + name + + " = " + generate_vertex_getter(name, type);
    };

    for (const auto& [index, type] : input_type) {
        context_assignment += get_assignment(name_map.at(index), type);
    }

    if (type == InstanceType::Instanced) {
        context_assignment += "vctx.instance_id = gl_InstanceID;\n";
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
        if (type == DataType::Int ||
            type == DataType::IVec4 ||
            type == DataType::Uint) {
            return "flat " + getDataTypeString(type) + " " + name + ";\n";
        }
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

std::string MaterialShaderDefineReplacer::getFragmentContextDeclaration(
    const Material& material,
    const RendererSettings& settings,
    InstanceType type
)
{
    std::string context_declaration = "struct VertexContext {\n";

    auto input_type = getInputTypeFromInstanceType(type);
    auto name_map = getNameMappingFromInstanceType(type);

    auto generate_attribute_decl = [] (const std::string& name, DataType type) {
        return getDataTypeString(type) + " " + name + ";\n";
    };

    for (const auto& [index, type] : input_type) {
        context_declaration += generate_attribute_decl(name_map.at(index), type);
    }

    // additional vertex context parameters
    context_declaration += "vec4 world_position;\n";

    if (settings.normal_mapping && material.getProperties().count(Property::Normal)) {
        context_declaration += "mat3 TBN;\n";
    }

    if (type == InstanceType::Instanced) {
        context_declaration += "int instance_id;\n";
    }

    context_declaration += "};\n";

    return context_declaration;
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

std::string MaterialShaderDefineReplacer::getVertexContextInterfaceBlockInGetters(
    const Material& material,
    const RendererSettings& settings,
    InstanceType type
)
{
    std::string getters;

    auto input_type = getInputTypeFromInstanceType(type);
    auto name_map = getNameMappingFromInstanceType(type);

    auto to_camel_case = [] (const std::string& input)
    {
        std::string result;
        bool capitalizeNext = true;

        for (char ch : input) {
            if (ch == '_') {
                capitalizeNext = true;
            } else {
                if (capitalizeNext) {
                    result += std::toupper(ch);
                    capitalizeNext = false;
                } else {
                    result += ch;
                }
            }
        }

        if (!result.empty()) {
            result[0] = std::toupper(result[0]);
        }

        return result;
    };

    auto generate_vertex_getter = [to_camel_case] (const std::string& name, DataType type) {
        return getDataTypeString(type) + " getVertex" + to_camel_case(name) + "() {\n\treturn _in_vertex_context." + name + ";\n}";
    };

    for (const auto& [index, type] : input_type) {
        getters += generate_vertex_getter(name_map.at(index), type);
    }

    // additional vertex context parameters
    getters += "vec4 getVertexWorldPosition() {\n\treturn _in_vertex_context.world_position;\n}";

    if (settings.normal_mapping && material.getProperties().count(Property::Normal)) {
        getters += "mat3 getVertexTBN() {\n\treturn _in_vertex_context.TBN;\n}";
    }

    if (type == InstanceType::Instanced) {
        getters += "int getVertexInstanceId() {\n\treturn _in_vertex_context.instance_id;\n}";
    }

    return getters;
}

std::string MaterialShaderDefineReplacer::getFragmentVertexContextCompute(
    const Material& material,
    const RendererSettings& settings,
    InstanceType type
) {
    std::string context = "VertexContext computeVertexContext() {\nVertexContext vctx;\n";

    auto input_type = getInputTypeFromInstanceType(type);
    auto name_map = getNameMappingFromInstanceType(type);

    auto to_camel_case = [] (const std::string& input)
    {
        std::string result;
        bool capitalizeNext = true;

        for (char ch : input) {
            if (ch == '_') {
                capitalizeNext = true;
            } else {
                if (capitalizeNext) {
                    result += std::toupper(ch);
                    capitalizeNext = false;
                } else {
                    result += ch;
                }
            }
        }

        if (!result.empty()) {
            result[0] = std::toupper(result[0]);
        }

        return result;
    };

    auto generate_vertex_getter = [to_camel_case] (const std::string& name, DataType type) {
        return "getVertex" + to_camel_case(name) + "();\n";
    };

    auto get_assignment = [generate_vertex_getter] (const std::string& name, DataType type) {
        return "vctx." + name + + " = " + generate_vertex_getter(name, type);
    };

    for (const auto& [index, type] : input_type) {
        context += get_assignment(name_map.at(index), type);
    }

    // additional vertex context parameters
    context += "vctx.world_position = getVertexWorldPosition();\n";

    if (settings.normal_mapping && material.getProperties().count(Property::Normal)) {
        context += "vctx.TBN = getVertexTBN();\n";
    }

    if (type == InstanceType::Instanced) {
        context += "vctx.instance_id = getVertexInstanceId();\n";
    }

    context += "\treturn vctx;\n}\n";

    return context;
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
    pass_through += "_out_vertex_context.world_position = ectx.world_position;\n";

    if (settings.normal_mapping && material.getProperties().count(Property::Normal)) {
        pass_through += "_out_vertex_context.TBN = ectx.TBN;\n";
    }

    if (type == InstanceType::Instanced) {
        pass_through += "_out_vertex_context.instance_id = vctx.instance_id;\n";
    }

    return pass_through;
}

