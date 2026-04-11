#include "cgltf_write.h"
#include <stb_image.h>

#include <limitless/loaders/gltf_model_saver.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/core/indexed_stream.hpp>
#include <limitless/core/uniform/uniform_value.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/ms/material.hpp>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <iostream>

using namespace Limitless;

template <typename T>
T* safeMalloc(size_t count = 1) {
    T* ptr = static_cast<T*>(malloc(sizeof(T) * count));
    if (ptr == nullptr) {
        throw ModelSaveError("Failed to allocate memory for " + std::string(typeid(T).name()));
    }
    memset(ptr, 0, sizeof(T) * count);
    return ptr;
}

template <typename T>
T* safeAllocOneMore(T** ptr, size_t* curr_count) {
    const auto prev_count = *curr_count;
    const auto new_count = prev_count + 1;
    T* result = *ptr + prev_count;
    *curr_count = new_count;
    return result;
}


// static std::string base64Encode(const std::vector<std::byte>& data) {
// 	const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	
// 	if (data.empty()) {
// 		return "";
// 	}
	
// 	std::string result;
// 	result.reserve(((data.size() + 2) / 3) * 4);
	
// 	for (size_t i = 0; i < data.size(); i += 3) {
// 		uint32_t value = 0;
// 		int padding = 0;
		
// 		// Pack 3 bytes into 24-bit value
// 		value |= static_cast<uint32_t>(data[i]) << 16;
// 		if (i + 1 < data.size()) {
// 			value |= static_cast<uint32_t>(data[i + 1]) << 8;
// 		} else {
// 			padding = 2;
// 		}
// 		if (i + 2 < data.size()) {
// 			value |= static_cast<uint32_t>(data[i + 2]);
// 		} else if (padding == 0) {
// 			padding = 1;
// 		}
		
// 		// Extract 4 base64 characters
// 		result += base64_chars[(value >> 18) & 0x3F];
// 		result += base64_chars[(value >> 12) & 0x3F];
// 		result += (padding >= 2) ? '=' : base64_chars[(value >> 6) & 0x3F];
// 		result += (padding >= 1) ? '=' : base64_chars[value & 0x3F];
// 	}
	
// 	return result;
// }

static cgltf_texture* cgltfTextureFrom(const Texture& texture, const std::string& name, cgltf_data* data, std::vector<unsigned char>* embed_buffer) {
    auto* texture_data = safeAllocOneMore(&data->textures, &data->textures_count);
    texture_data->name = strdup(name.c_str());

    std::cout << "saving texture " << texture_data->name << '\n';

    texture_data->image = safeAllocOneMore(&data->images, &data->images_count);
    const auto& maybe_path = texture.getPath();
    std::string mime_type;
    if (!embed_buffer) {
        if (!maybe_path) {
            // not supported because we need to write texture data in a format supported by stbi (png, jpg, etc.)
            throw ModelSaveError("Texture " + name + " has no path");
        }
        // mime_type = "image/" + maybe_path->extension().string().erase(0, 1);
        texture_data->image->uri = strdup(maybe_path->string().c_str());
        std::cout << "saving image of " << texture_data->image->uri << '\n';

    } else {
        if (!maybe_path) {
            throw ModelSaveError("base64 encoding not supported yet");
        } else {
            auto file = std::fstream {*maybe_path, std::ios::in | std::ios::binary};
            if (!file.is_open()) {
                throw ModelSaveError("failed to open " + maybe_path->string() + " for embedding");
            }

            std::cout << "embedding " + maybe_path->string() + "\n";

            const auto buffer_start_pos = embed_buffer->size();

            char buffer[8192];
            while (file) {
                file.read(buffer, sizeof(buffer));
                const auto read = file.gcount();
                const auto prev_buffer_size = embed_buffer->size();
                embed_buffer->resize(prev_buffer_size + read);
                std::memcpy(embed_buffer->data() + prev_buffer_size, buffer, read);
            }
            
            auto* buffer_view = safeAllocOneMore(&data->buffer_views, &data->buffer_views_count);
            buffer_view->offset = buffer_start_pos;
            buffer_view->buffer = data->buffers; // TODO: this might not be populated yet.
            buffer_view->size = embed_buffer->size() - buffer_start_pos;
            texture_data->image->buffer_view = buffer_view;
            texture_data->image->mime_type = strdup(("image/" + maybe_path->extension().string().erase(0, 1)).c_str());

            std::cout << "total " << buffer_view->size << " bytes\n";
        }
    }

    texture_data->image->name = strdup((name + "_image").c_str());
    // texture_data->image->mime_type = strdup("image/png"); // TODO: determine mime type from texture

    auto*& sampler = texture_data->sampler;
    sampler = safeAllocOneMore(&data->samplers, &data->samplers_count);
    sampler->name = strdup((name + "_sampler").c_str());

    auto cgltfFilterTypeFrom = [](Texture::Filter filter) -> cgltf_filter_type {
        switch (filter) {
        case Texture::Filter::Linear:
            return cgltf_filter_type_linear;
        case Texture::Filter::LinearMipmapLinear:
            return cgltf_filter_type_linear_mipmap_linear;
        case Texture::Filter::LinearMipMapNearest:
            return cgltf_filter_type_linear_mipmap_nearest;
        case Texture::Filter::Nearest:
            return cgltf_filter_type_nearest;
        case Texture::Filter::NearestMipmapNearest:
            return cgltf_filter_type_nearest_mipmap_nearest;
        case Texture::Filter::NearestMipMapLinear:
            return cgltf_filter_type_nearest_mipmap_linear;
        }
        throw ModelSaveError("Unsupported texture filter: " + std::to_string(static_cast<int>(filter)));
    };

    auto cgltfWrapModeFrom = [](Texture::Wrap wrap) -> cgltf_wrap_mode {
        switch (wrap) {
        case Texture::Wrap::ClampToEdge:
            return cgltf_wrap_mode_clamp_to_edge;
        case Texture::Wrap::MirroredRepeat:
            return cgltf_wrap_mode_mirrored_repeat;
        case Texture::Wrap::Repeat:
            return cgltf_wrap_mode_repeat;
        case Texture::Wrap::ClampToBorder:
            return cgltf_wrap_mode_clamp_to_edge;
        }
        throw ModelSaveError("Unsupported texture wrap mode: " + std::to_string(static_cast<int>(wrap)));
    };

    sampler->mag_filter = cgltfFilterTypeFrom(texture.getMag());
    sampler->min_filter = cgltfFilterTypeFrom(texture.getMin());
    sampler->wrap_s = cgltfWrapModeFrom(texture.getWrapS());
    sampler->wrap_t = cgltfWrapModeFrom(texture.getWrapT());

    return texture_data;
}

static cgltf_data* makeData(const Model& model, bool embed_textures) {
    cgltf_data* data = safeMalloc<cgltf_data>();
    data->buffers_count = 1;
    data->buffers = safeMalloc<cgltf_buffer>();

    data->asset.generator = strdup("limitless GLTF model saver");
    data->asset.version = strdup("2.0");

    std::vector<unsigned char> buffer;
    auto* embed_buffer = embed_textures ? &buffer : nullptr;

    data->meshes_count = model.getMeshes().size();
    data->meshes = safeMalloc<cgltf_mesh>(data->meshes_count);

    data->materials_count = model.getMaterials().size();
    data->materials = safeMalloc<cgltf_material>(data->materials_count);

    const auto accessors_per_mesh = 4; // position, normal, uv, indices
    data->accessors_count = data->meshes_count * accessors_per_mesh;
    data->accessors = safeMalloc<cgltf_accessor>(data->accessors_count);

    data->scenes_count = 1;
    data->scenes = safeMalloc<cgltf_scene>();
    data->scene = data->scenes;
    data->scene->name = strdup("scene");

    data->nodes_count = data->meshes_count;
    data->nodes = safeMalloc<cgltf_node>(data->nodes_count);
    data->scene->nodes_count = data->nodes_count;
    data->scene->nodes = safeMalloc<cgltf_node*>(data->nodes_count);

    for (size_t i = 0; i < data->nodes_count; ++i) {
        auto& node_data = data->nodes[i];
        node_data.mesh = &data->meshes[i];
        node_data.name = strdup(("mesh" + std::to_string(i)).c_str());
        data->scene->nodes[i] = &data->nodes[i];
    }

    data->buffer_views = safeMalloc<cgltf_buffer_view>(data->meshes_count * (2 + 4)); // vertices, indices and up to 4 textures

    data->images = safeMalloc<cgltf_image>(16);
    data->textures = safeMalloc<cgltf_texture>(16);
    data->samplers = safeMalloc<cgltf_sampler>(16);

    for (size_t i = 0; i < model.getMeshes().size(); ++i) {
        const auto& amesh = model.getMeshes()[i];
        const auto* mesh = dynamic_cast<const Mesh*>(amesh.get());
        if (mesh == nullptr) {
            continue;
        }
        const auto& material = model.getMaterials()[i];
        
        const auto& stream = mesh->getVertexStream();
        const auto* indexed_stream = dynamic_cast<const IndexedVertexStream<VertexNormalTangent>*>(&stream);
        if (indexed_stream == nullptr) {
            continue;
        }
        const auto& vertices = indexed_stream->getVertices();
        const auto& indices = indexed_stream->getIndices();

        struct GltfVertex {
            glm::vec3 position;
            glm::vec3 normal;
            // glm::vec4 tangent; // this differs from VertexNormalTangent, which has glm::vec3.
            glm::vec2 texCoord0;
        };

        auto min_position = glm::vec3(std::numeric_limits<float>::max());
        auto max_position = glm::vec3(std::numeric_limits<float>::min());

        std::vector<GltfVertex> gltf_vertices;
        for (const auto& vertice : vertices) {
            min_position.x = std::min(min_position.x, vertice.position.x);
            min_position.y = std::min(min_position.y, vertice.position.y);
            min_position.z = std::min(min_position.z, vertice.position.z);

            max_position.x = std::max(max_position.x, vertice.position.x);
            max_position.y = std::max(max_position.y, vertice.position.y);
            max_position.z = std::max(max_position.z, vertice.position.z);

            gltf_vertices.push_back(GltfVertex{
                vertice.position,
                vertice.normal,
                // glm::vec4(vertice.tangent, 1.f), // w indicates handedness, -1 or +1 (no idea whats that)
                vertice.uv
            });
        }

        const auto alignment = 4;
        if (buffer.size() % alignment != 0) {
            const auto padding_size = alignment - (buffer.size() % alignment);
            buffer.resize(buffer.size() + padding_size);
            std::memset(buffer.data() + buffer.size() - padding_size, 0, padding_size);
        }
        const auto vertices_pos = buffer.size();
        const auto vertices_bytes = gltf_vertices.size() * sizeof(GltfVertex);
        const auto indices_bytes = indices.size() * sizeof(uint32_t);
        buffer.resize(buffer.size() + vertices_bytes + indices_bytes);
        std::memcpy(buffer.data() + vertices_pos, gltf_vertices.data(), vertices_bytes);
        const auto indices_pos = vertices_pos + vertices_bytes;
        std::memcpy(buffer.data() + indices_pos, indices.data(), indices_bytes);

        auto* mesh_data = &data->meshes[i];
        mesh_data->name = strdup(mesh->getName().c_str());
        mesh_data->primitives_count = 1;
        mesh_data->primitives = safeMalloc<cgltf_primitive>();

        std::cout << "mesh->getName(): " << mesh->getName() << std::endl;
        std::cout << "vertices.size(): " << vertices.size() << std::endl;
        std::cout << "indices.size(): " << indices.size() << std::endl;
        std::cout << "buffer.size(): " << buffer.size() << std::endl;

        auto& primitive = mesh_data->primitives[0];
        primitive.type = cgltf_primitive_type_triangles;
        primitive.attributes_count = 3;
        primitive.attributes = safeMalloc<cgltf_attribute>(primitive.attributes_count);

        auto& position = primitive.attributes[0];
        position.name = strdup("POSITION");
        position.type = cgltf_attribute_type_position;
        position.index = 0;
        position.data = &data->accessors[accessors_per_mesh*i + 0];
        position.data->component_type = cgltf_component_type_r_32f;
        position.data->type = cgltf_type_vec3;
        position.data->count = gltf_vertices.size();
        position.data->normalized = false;
        position.data->offset = 0;
        position.data->stride = sizeof(GltfVertex);
        // vertex position attribute accessors MUST have accessor.min and accessor.max defined. 
        position.data->has_min = true;
        position.data->min[0] = min_position[0];
        position.data->min[1] = min_position[1];
        position.data->min[2] = min_position[2];
        position.data->has_max = true;
        position.data->max[0] = max_position[0];
        position.data->max[1] = max_position[1];
        position.data->max[2] = max_position[2];

        position.data->buffer_view = safeAllocOneMore(&data->buffer_views, &data->buffer_views_count);
        auto*& vertex_buffer_view = position.data->buffer_view;
        vertex_buffer_view->buffer = data->buffers;
        vertex_buffer_view->offset = vertices_pos;
        vertex_buffer_view->size = gltf_vertices.size() * sizeof(GltfVertex);
        vertex_buffer_view->stride = sizeof(GltfVertex);
        vertex_buffer_view->type = cgltf_buffer_view_type_vertices;
        
        auto& normal = primitive.attributes[1];
        normal.name = strdup("NORMAL");
        normal.type = cgltf_attribute_type_normal;
        normal.index = 1;
        normal.data = &data->accessors[accessors_per_mesh*i + normal.index];
        normal.data->component_type = cgltf_component_type_r_32f;
        normal.data->count = gltf_vertices.size();
        normal.data->type = cgltf_type_vec3;
        normal.data->offset = offsetof(GltfVertex, GltfVertex::normal);
        normal.data->stride = sizeof(GltfVertex);
        normal.data->buffer_view = vertex_buffer_view;

        // auto& tangent = primitive.attributes[2];
        // tangent.name = strdup("TANGENT");
        // tangent.type = cgltf_attribute_type_tangent;
        // tangent.index = 2;
        // tangent.data = &data->accessors[accessors_per_mesh*i + 2];
        // tangent.data->component_type = cgltf_component_type_r_32f;
        // tangent.data->count = gltf_vertices.size();
        // tangent.data->type = cgltf_type_vec4;
        // tangent.data->normalized = false;
        // tangent.data->offset = offsetof(GltfVertex, GltfVertex::tangent);
        // tangent.data->stride = sizeof(GltfVertex);
        // tangent.data->buffer_view = vertex_buffer_view;

        auto& uv = primitive.attributes[2];
        uv.name = strdup("TEXCOORD_0");
        uv.type = cgltf_attribute_type_texcoord;
        uv.index = 2;
        uv.data = &data->accessors[accessors_per_mesh*i + uv.index];
        uv.data->component_type = cgltf_component_type_r_32f;
        uv.data->count = gltf_vertices.size();
        uv.data->type = cgltf_type_vec2;
        uv.data->normalized = false;
        uv.data->offset = offsetof(GltfVertex, GltfVertex::texCoord0);
        uv.data->stride = sizeof(GltfVertex);
        uv.data->buffer_view = vertex_buffer_view;

        primitive.indices = &data->accessors[accessors_per_mesh*i + 3];
        primitive.indices->component_type = cgltf_component_type_r_32u;
        primitive.indices->type = cgltf_type_scalar;
        primitive.indices->count = indices.size();
        primitive.indices->stride = sizeof(uint32_t);
        primitive.indices->buffer_view = safeAllocOneMore(&data->buffer_views, &data->buffer_views_count);
        primitive.indices->buffer_view->buffer = data->buffers;
        primitive.indices->buffer_view->offset = indices_pos;
        primitive.indices->buffer_view->size = indices.size() * sizeof(uint32_t);
        // primitive.indices->buffer_view->stride = sizeof(uint32_t);
        primitive.indices->buffer_view->type = cgltf_buffer_view_type_indices;

        primitive.material = &data->materials[i];
        std::cout << "saving material " << material->getName() << '\n';
        primitive.material->name = strdup((material->getName()).c_str());
        auto& pmaterial = *primitive.material;
        for (const auto& property : material->getProperties()) {
            switch (property.first) {
            case ms::Property::Color: {
                const auto& color = static_cast<const UniformValue<glm::vec4>&>(*property.second).getValue();
                pmaterial.has_pbr_metallic_roughness = true;
                pmaterial.pbr_metallic_roughness.base_color_factor[0] = color.r;
                pmaterial.pbr_metallic_roughness.base_color_factor[1] = color.g;
                pmaterial.pbr_metallic_roughness.base_color_factor[2] = color.b;
                pmaterial.pbr_metallic_roughness.base_color_factor[3] = color.a;
                break;
            }
            case ms::Property::EmissiveColor: {
                const auto& emissive_color = static_cast<const UniformValue<glm::vec3>&>(*property.second).getValue();
                pmaterial.emissive_factor[0] = emissive_color.r;
                pmaterial.emissive_factor[1] = emissive_color.g;
                pmaterial.emissive_factor[2] = emissive_color.b;
                break;
            }
            case ms::Property::Diffuse: {
                const auto& diffuse_texture = static_cast<const UniformSampler&>(*property.second).getSampler();
                auto& color_texture_view = pmaterial.pbr_metallic_roughness.base_color_texture;
                pmaterial.has_pbr_metallic_roughness = true;
                color_texture_view.texture = cgltfTextureFrom(*diffuse_texture, material->getName() + property.second->getName(), data, embed_buffer);
                color_texture_view.texcoord = 0;
                color_texture_view.scale = 1.0f;
                color_texture_view.has_transform = false;
            }
                break;

            case ms::Property::Normal: {
                const auto& normal_texture = static_cast<const UniformSampler&>(*property.second).getSampler();
                auto& normal_texture_view = pmaterial.normal_texture;
                normal_texture_view.texture = cgltfTextureFrom(*normal_texture, material->getName() + property.second->getName(), data, embed_buffer);
                normal_texture_view.texcoord = 0;
                normal_texture_view.scale = 1.0f;
                normal_texture_view.has_transform = false;
            }
                break;
            case ms::Property::EmissiveMask: {
                const auto& emissive_mask_texture = static_cast<const UniformSampler&>(*property.second).getSampler();
                auto& emissive_mask_texture_view = pmaterial.emissive_texture;
                emissive_mask_texture_view.texture = cgltfTextureFrom(*emissive_mask_texture, material->getName() + property.second->getName(), data, embed_buffer);
                emissive_mask_texture_view.texcoord = 0;
                emissive_mask_texture_view.scale = 1.f;
                emissive_mask_texture_view.has_transform = false;
            }
                break;
            case ms::Property::BlendMask:
                // not supported yet
                break;
            case ms::Property::MetallicTexture:
                // not supported yet
                break;
            case ms::Property::RoughnessTexture:
                // not supported yet
                break;
            case ms::Property::AmbientOcclusionTexture: 
                // not supported yet
                break;
            case ms::Property::ORM: 
                // not supported yet
                break;
            case ms::Property::Metallic: 
                // not supported yet
                break;
            case ms::Property::Roughness: 
                // not supported yet
                break;
            case ms::Property::IoR: 
                pmaterial.has_ior = true;
                pmaterial.ior.ior = static_cast<const UniformValue<float>&>(*property.second).getValue();
                break;
            case ms::Property::Absorption: 
                // not supported yet
                break;
            case ms::Property::MicroThickness: 
                // not supported yet
                break;
            case ms::Property::Thickness: 
                // not supported yet
                break;
            case ms::Property::Reflectance: 
                // not supported yet
                break;
            case ms::Property::Transmission: 
                // not supported yet
                break;
            default:
                throw ModelSaveError("Unsupported material property: " + std::to_string(static_cast<int>(property.first)));
            }
        }
        pmaterial.alpha_mode = [&]() {
            switch (material->getBlending()) {
            case ms::Blending::Opaque: {
                const auto alpha_uniform_it = material->getUniforms().find("alpha_cutoff");
                if (alpha_uniform_it != material->getUniforms().end()) {
                    const auto& alpha_cutoff = static_cast<const UniformValue<float>&>(*alpha_uniform_it->second).getValue();
                    pmaterial.alpha_cutoff = alpha_cutoff;
                    return cgltf_alpha_mode_mask;
                } else {
                    return cgltf_alpha_mode_opaque;
                }
            }
            case ms::Blending::Translucent:
                return cgltf_alpha_mode_blend;
            case ms::Blending::Additive:
                throw ModelSaveError("Additive blending is not supported yet");
            case ms::Blending::Modulate:
                throw ModelSaveError("Modulate blending is not supported yet");
            case ms::Blending::Text:
                throw ModelSaveError("Text blending is not supported");
            }
            throw ModelSaveError("Unsupported blending mode: " + std::to_string(static_cast<int>(material->getBlending())));
        }();
        pmaterial.double_sided = material->getTwoSided();
        pmaterial.unlit = material->getShading() == ms::Shading::Unlit;

        pmaterial.fragment = material->getFragmentSnippet() != "" ? strdup(material->getFragmentSnippet().c_str()) : nullptr;
        pmaterial.models = [&](){
            unsigned int result = 0;
            for (const auto instance_type : material->getModelShaders()) {
                switch (instance_type) {
                    case Limitless::InstanceType::Model:
                        result |= CGLTF_LIMITLESS_MATERIAL_MODELS_MODEL;
                        continue;

                    case Limitless::InstanceType::Instanced:
                        result |= CGLTF_LIMITLESS_MATERIAL_MODELS_INSTANCED;
                        continue;

                    case Limitless::InstanceType::Decal:
                    case Limitless::InstanceType::Effect:
                    case Limitless::InstanceType::Skeletal:
                    case Limitless::InstanceType::SkeletalInstanced:
                    case Limitless::InstanceType::Terrain:
                        throw ModelSaveError("unsupported material instance type");
                }
                throw ModelSaveError("unknown material instance type");
            }
            return result;
        }();
        
        pmaterial.uniforms_count = material->getUniforms().size();
        pmaterial.uniforms = safeMalloc<cgltf_uniform>(pmaterial.uniforms_count);
        size_t uniform_index = 0;
        for (const auto& [uniform_name, uniform_value] : material->getUniforms()) {
            auto& uniform_data = pmaterial.uniforms[uniform_index];
            uniform_data.name = strdup(uniform_name.c_str());

            std::cout << "saving uniform " << uniform_name << '\n';

            uniform_data.type = [&]() {
                switch (uniform_value->getType()) {
                case UniformType::Value:
                    return cgltf_uniform_type_value;
                case UniformType::Sampler:
                    return cgltf_uniform_type_sampler;
                case UniformType::Time:
                    return cgltf_uniform_type_time;
                }
                throw ModelSaveError("Unsupported uniform type: " + std::to_string(static_cast<int>(uniform_value->getType())));
            }();

            if (uniform_value->getType() == UniformType::Sampler) {
                uniform_data.value_type = cgltf_uniform_value_type_texture;
                uniform_data.value.texture_value = cgltfTextureFrom(*static_cast<const UniformSampler&>(*uniform_value).getSampler().get(), uniform_name, data, embed_buffer);
            } else switch (uniform_value->getValueType()) {
                case UniformValueType::Float:
                    uniform_data.value_type = cgltf_uniform_value_type_float;
                    uniform_data.value.float_value = static_cast<const UniformValue<float>&>(*uniform_value).getValue();
                    break;
                case UniformValueType::Int:
                    uniform_data.value_type = cgltf_uniform_value_type_int;
                    uniform_data.value.int_value = static_cast<const UniformValue<int32_t>&>(*uniform_value).getValue();
                    break;
                case UniformValueType::Uint:
                    uniform_data.value_type = cgltf_uniform_value_type_uint;
                    uniform_data.value.uint_value = static_cast<const UniformValue<uint32_t>&>(*uniform_value).getValue();
                    break;
                case UniformValueType::Vec2: {
                    uniform_data.value_type = cgltf_uniform_value_type_vec2;
                    const auto& vec2 = static_cast<const UniformValue<glm::vec2>&>(*uniform_value).getValue();
                    uniform_data.value.vec2_value[0] = vec2.x;
                    uniform_data.value.vec2_value[1] = vec2.y;
                    break;
                }
                case UniformValueType::Vec3: {
                    uniform_data.value_type = cgltf_uniform_value_type_vec3;
                    const auto& vec3 = static_cast<const UniformValue<glm::vec3>&>(*uniform_value).getValue();
                    uniform_data.value.vec3_value[0] = vec3.x;
                    uniform_data.value.vec3_value[1] = vec3.y;
                    uniform_data.value.vec3_value[2] = vec3.z;
                    break;
                }
                case UniformValueType::Vec4: {
                    uniform_data.value_type = cgltf_uniform_value_type_vec4;
                    const auto& vec4 = static_cast<const UniformValue<glm::vec4>&>(*uniform_value).getValue();
                    uniform_data.value.vec4_value[0] = vec4.x;
                    uniform_data.value.vec4_value[1] = vec4.y;
                    uniform_data.value.vec4_value[2] = vec4.z;
                    uniform_data.value.vec4_value[3] = vec4.w;
                    break;
                }
                case UniformValueType::Mat3: {
                    uniform_data.value_type = cgltf_uniform_value_type_mat3;
                    const auto& mat3 = static_cast<const UniformValue<glm::mat3>&>(*uniform_value).getValue();
                    uniform_data.value.mat3_value[0][0] = mat3[0][0];
                    uniform_data.value.mat3_value[0][1] = mat3[0][1];
                    uniform_data.value.mat3_value[0][2] = mat3[0][2];
                    uniform_data.value.mat3_value[1][0] = mat3[1][0];
                    uniform_data.value.mat3_value[1][1] = mat3[1][1];
                    uniform_data.value.mat3_value[1][2] = mat3[1][2];
                    uniform_data.value.mat3_value[2][0] = mat3[2][0];
                    uniform_data.value.mat3_value[2][1] = mat3[2][1];
                    uniform_data.value.mat3_value[2][2] = mat3[2][2];
                    break;
                }
                case UniformValueType::Mat4: {
                    uniform_data.value_type = cgltf_uniform_value_type_mat4;
                    const auto& mat4 = static_cast<const UniformValue<glm::mat4>&>(*uniform_value).getValue();
                    uniform_data.value.mat4_value[0][0] = mat4[0][0];
                    uniform_data.value.mat4_value[0][1] = mat4[0][1];
                    uniform_data.value.mat4_value[0][2] = mat4[0][2];
                    uniform_data.value.mat4_value[0][3] = mat4[0][3];
                    uniform_data.value.mat4_value[1][0] = mat4[1][0];
                    uniform_data.value.mat4_value[1][1] = mat4[1][1];
                    uniform_data.value.mat4_value[1][2] = mat4[1][2];
                    uniform_data.value.mat4_value[1][3] = mat4[1][3];
                    uniform_data.value.mat4_value[2][0] = mat4[2][0];
                    uniform_data.value.mat4_value[2][1] = mat4[2][1];
                    uniform_data.value.mat4_value[2][2] = mat4[2][2];
                    uniform_data.value.mat4_value[2][3] = mat4[2][3];
                    uniform_data.value.mat4_value[3][0] = mat4[3][0];
                    uniform_data.value.mat4_value[3][1] = mat4[3][1];
                    uniform_data.value.mat4_value[3][2] = mat4[3][2];
                    uniform_data.value.mat4_value[3][3] = mat4[3][3];
                    break;
                }
                default:
                    throw ModelSaveError("Unsupported uniform value type: " + std::to_string(static_cast<int>(uniform_value->getValueType())));
            }
            uniform_index++;
        }
    }
    auto* bin = static_cast<unsigned char*>(malloc(buffer.size()));
    if (bin == nullptr) {
        throw ModelSaveError("Failed to allocate memory for binary data");
    }
    std::memcpy(bin, buffer.data(), buffer.size());
    data->bin_size = buffer.size();
    data->bin = bin;
    data->buffers[0].data = bin;
    data->buffers[0].size = buffer.size();
    return data;
}

void GltfModelSaver::saveModel(const std::filesystem::path& output_path, const Model& model) {
    cgltf_options options = cgltf_options {
		cgltf_file_type_glb,
		0, // auto json token count
		cgltf_memory_options {nullptr, nullptr, nullptr},
		cgltf_file_options {nullptr, nullptr, nullptr}
    };
    cgltf_data* data = makeData(model, /* embed_textures = */ true);
    if (data == nullptr) {
        throw ModelSaveError("Failed to create data for model");
    }
    cgltf_result result = cgltf_write_file(&options, output_path.string().c_str(), data);
    if (result != cgltf_result_success)
    {
        throw ModelSaveError("Failed to write model to file: " + std::to_string(static_cast<int>(result)));
    }
}
