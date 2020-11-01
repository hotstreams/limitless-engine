#include <material_system/custom_material.hpp>
#include <core/bindless_texture.hpp>
#include <core/extension_texture.hpp>
#include <core/texture.hpp>
#include <cstring>
#include <core/context_initializer.hpp>
#include <core/buffer.hpp>

using namespace GraphicsEngine;

void CustomMaterial::update() const noexcept {
    // checks if there are values need to be mapped into material buffer
    bool changed = false;
    for (const auto& [type, uniform] : properties) {
        if (uniform->getChanged()) {
            changed = true;
            break;
        }
    }

    for (const auto& [name, uniform] : uniforms) {
        if (uniform->getChanged()) {
            changed = true;
            break;
        }
    }

    if (!changed) return;

    std::vector<std::byte> data(material_buffer->getSize());

    for (const auto& [type, prop] : properties) {
        switch (type) {
            case PropertyType::Color:
            case PropertyType::EmissiveColor: {
                auto& uniform = static_cast<UniformValue<glm::vec4>&>(*prop);
                auto offset = uniform_offsets.at(uniform.getName());
                std::memcpy(data.data() + offset, &uniform.getValue(), sizeof(glm::vec4));
                break;
            }
            case PropertyType::Shininess:
            case PropertyType::Metallic:
            case PropertyType::Roughness: {
                auto& uniform = static_cast<UniformValue<float> &>(*prop);
                auto offset = uniform_offsets.at(uniform.getName());
                std::memcpy(data.data() + offset, &uniform.getValue(), sizeof(float));
                break;
            }
            case PropertyType::Diffuse:
            case PropertyType::Specular:
            case PropertyType::Normal:
            case PropertyType::Displacement:
            case PropertyType::EmissiveMask:
            case PropertyType::BlendMask:
            case PropertyType::MetallicTexture:
            case PropertyType::RoughnessTexture:
                if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                    auto& uniform = static_cast<UniformSampler&>(*prop);
                    auto offset = uniform_offsets.at(uniform.getName());
                    auto& texture = uniform.getSampler();
                    std::memcpy(data.data() + offset, &static_cast<const BindlessTexture&>(texture->getExtensionTexture()).getHandle(), sizeof(uint64_t));
                }
                break;
        }
    }

    for (const auto& [name, uniform] : uniforms) {
        switch (uniform->getType()) {
            case UniformType::Value:
                switch (uniform->getValueType()) {
                    case UniformValueType::Uint: {
                        auto &uniform_value = static_cast<UniformValue<unsigned int>&>(*uniform);
                        auto offset = uniform_offsets.at(name);
                        std::memcpy(data.data() + offset, &uniform_value.getValue(), sizeof(unsigned int));
                        break;
                    }
                    case UniformValueType::Int: {
                        auto &uniform_value = static_cast<UniformValue<int>&>(*uniform);
                        auto offset = uniform_offsets.at(name);
                        std::memcpy(data.data() + offset, &uniform_value.getValue(), sizeof(int));
                        break;
                    }
                    case UniformValueType::Float: {
                        auto &uniform_value = static_cast<UniformValue<float>&>(*uniform);
                        auto offset = uniform_offsets.at(name);
                        std::memcpy(data.data() + offset, &uniform_value.getValue(), sizeof(float));
                        break;
                    }
                    case UniformValueType::Vec2: {
                        auto &uniform_value = static_cast<UniformValue<glm::vec2>&>(*uniform);
                        auto offset = uniform_offsets.at(name);
                        std::memcpy(data.data() + offset, &uniform_value.getValue(), sizeof(glm::vec2));
                        break;
                    }
                    case UniformValueType::Vec3: {
                        auto &uniform_value = static_cast<UniformValue<glm::vec3>&>(*uniform);
                        auto offset = uniform_offsets.at(name);
                        std::memcpy(data.data() + offset, &uniform_value.getValue(), sizeof(glm::vec3));
                        break;
                    }
                    case UniformValueType::Vec4: {
                        auto &uniform_value = static_cast<UniformValue<glm::vec4>&>(*uniform);
                        auto offset = uniform_offsets.at(name);
                        std::memcpy(data.data() + offset, &uniform_value.getValue(), sizeof(glm::vec4));
                        break;
                    }
                    case UniformValueType::Mat4: {
                        auto &uniform_value = static_cast<UniformValue<glm::mat4>&>(*uniform);
                        auto offset = uniform_offsets.at(name);
                        std::memcpy(data.data() + offset, &uniform_value.getValue(), sizeof(glm::mat4));
                        break;
                    }
                    case UniformValueType::Mat3: {
                        auto &uniform_value = static_cast<UniformValue<glm::mat3>&>(*uniform);
                        auto offset = uniform_offsets.at(name);
                        std::memcpy(data.data() + offset, &uniform_value.getValue(), sizeof(glm::mat3));
                        break;
                    }
                }
                break;
            case UniformType::Sampler:
                if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                    auto& uniform_sampler = static_cast<UniformSampler&>(*uniform);
                    auto offset = uniform_offsets.at(name);
                    auto& texture = uniform_sampler.getSampler();
                    std::memcpy(data.data() + offset, &static_cast<const BindlessTexture&>(texture->getExtensionTexture()).getHandle(), sizeof(uint64_t));
                }
                break;
        }
    }

    material_buffer->mapData(data.data(), data.size());

    for (const auto& [type, uniform] : properties) {
        uniform->getChanged() = false;
    }

    for (const auto& [name, uniform] : uniforms) {
        uniform->getChanged() = false;
    }
}

void GraphicsEngine::swap(CustomMaterial& lhs, CustomMaterial& rhs) noexcept {
    using std::swap;

    swap(static_cast<Material&>(lhs), static_cast<Material&>(rhs));
    swap(lhs.uniforms, rhs.uniforms);
}

CustomMaterial::CustomMaterial(const CustomMaterial& material)
    : Material{material} {
    uniforms.reserve(uniforms.size());
    for (const auto& [name, uniform] : uniforms) {
        uniforms.emplace(name, uniform->clone());
    }
}

CustomMaterial& CustomMaterial::operator=(CustomMaterial material) {
    swap(*this, material);
    return *this;
}

CustomMaterial* CustomMaterial::clone() const noexcept {
    return new CustomMaterial(*this);
}

CustomMaterial::CustomMaterial(CustomMaterial&& rhs) noexcept
    : CustomMaterial{} {
    swap(*this, rhs);
}

CustomMaterial& CustomMaterial::operator=(CustomMaterial&& rhs) noexcept {
    swap(*this, rhs);
    return *this;
}

CustomMaterial::CustomMaterial() noexcept : Material{} { }

