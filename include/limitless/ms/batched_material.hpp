#pragma once

#include <limitless/ms/material.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/uniform/uniform_value_array.hpp>
#include <limitless/core/uniform/uniform_time.hpp>
#include <limitless/core/texture/texture.hpp>

namespace Limitless::ms {
    class BatchedMaterial : public Material {
    private:
    public:
        BatchedMaterial(Builder& builder)
            : Material(builder) {

        }

        void set(size_t index, const Material& material, bool force = false) {
            if (!(*this == material)) {
                // materials type differ
                // check for equality
            }

            for (const auto& [property, uniform] : material.getProperties()) {
                const auto name = uniform->getName() + "_array";

                switch (property) {
                    case Property::Diffuse:
                    case Property::Normal:
                    case Property::EmissiveMask:
                    case Property::BlendMask:
                    case Property::MetallicTexture:
                    case Property::RoughnessTexture:
                    case Property::AmbientOcclusionTexture:
                    case Property::ORM: {
                        auto uniform_sampler = static_cast<UniformSampler&>(*uniforms[name]);
                        auto texture = uniform_sampler.getSampler();

                        if (uniform->isChanged() || force) {
                            Texture::copy(static_cast<UniformSampler&>(*uniform).getSampler(), index, texture);
                        }
                    }
                        break;

                    case Property::Color:
                    case Property::EmissiveColor: {
                        auto uniform_value = static_cast<UniformValueArray<glm::vec4>&>(*uniforms[name]);

                        if (uniform->isChanged() || force) {
                            uniform_value.setValue(index, static_cast<UniformValue<glm::vec4>&>(*uniform).getValue());
                        }
                    }
                        break;

                    case Property::Metallic:
                    case Property::Roughness:
                    case Property::IoR:
                    case Property::Absorption:
                    case Property::MicroThickness:
                    case Property::Thickness:
                    case Property::Reflectance:
                    case Property::Transmission:  {
                        auto uniform_value = static_cast<UniformValueArray<float>&>(*uniforms[name]);

                        if (uniform->isChanged() || force) {
                            uniform_value.setValue(index, static_cast<UniformValue<float>&>(*uniform).getValue());
                        }
                    }
                        break;
                }
            }

            for (const auto& [property, uniform] : material.getUniforms()) {
                const auto name = uniform->getName() + "_array";

                switch (uniform->getType()) {
                    case UniformType::Value:
                        switch (uniform->getValueType()) {
                            case DataType::Float: {
                                auto uniform_value = static_cast<UniformValueArray<float>&>(*uniforms[name]);

                                if (uniform->isChanged() || force) {
                                    uniform_value.setValue(index, static_cast<UniformValue<float>&>(*uniform).getValue());
                                }
                            }
                                break;
                            case DataType::Int: {
                                auto uniform_value = static_cast<UniformValueArray<int32_t>&>(*uniforms[name]);

                                if (uniform->isChanged() || force) {
                                    uniform_value.setValue(index, static_cast<UniformValue<int32_t>&>(*uniform).getValue());
                                }
                            }
                                break;
                            case DataType::Uint: {
                                auto uniform_value = static_cast<UniformValueArray<uint32_t>&>(*uniforms[name]);

                                if (uniform->isChanged() || force) {
                                    uniform_value.setValue(index, static_cast<UniformValue<uint32_t>&>(*uniform).getValue());
                                }
                            }
                                break;
                            case DataType::Vec2: {
                                auto uniform_value = static_cast<UniformValueArray<glm::vec2>&>(*uniforms[name]);

                                if (uniform->isChanged() || force) {
                                    uniform_value.setValue(index, static_cast<UniformValue<glm::vec2>&>(*uniform).getValue());
                                }
                            }
                                break;
                            case DataType::Vec3: {
                                auto uniform_value = static_cast<UniformValueArray<glm::vec3>&>(*uniforms[name]);

                                if (uniform->isChanged() || force) {
                                    uniform_value.setValue(index, static_cast<UniformValue<glm::vec3>&>(*uniform).getValue());
                                }
                            }
                                break;
                            case DataType::Vec4: {
                                auto uniform_value = static_cast<UniformValueArray<glm::vec4>&>(*uniforms[name]);

                                if (uniform->isChanged() || force) {
                                    uniform_value.setValue(index, static_cast<UniformValue<glm::vec4>&>(*uniform).getValue());
                                }
                            }
                                break;
                            case DataType::Mat3: {
                                auto uniform_value = static_cast<UniformValueArray<glm::mat3>&>(*uniforms[name]);

                                if (uniform->isChanged() || force) {
                                    uniform_value.setValue(index, static_cast<UniformValue<glm::mat3>&>(*uniform).getValue());
                                }
                            }
                                break;
                            case DataType::Mat4: {
                                auto uniform_value = static_cast<UniformValueArray<glm::mat3>&>(*uniforms[name]);

                                if (uniform->isChanged() || force) {
                                    uniform_value.setValue(index, static_cast<UniformValue<glm::mat4>&>(*uniform).getValue());
                                }
                            }
                                break;
                        }
                        break;

                    case UniformType::Sampler: {
                        auto uniform_sampler = static_cast<UniformSampler&>(*uniforms[name]);
                        auto texture = uniform_sampler.getSampler();

                        if (uniform->isChanged() || force) {
                            Texture::copy(static_cast<UniformSampler&>(*uniform).getSampler(), index, texture);
                        }
                    }
                        break;
                    case UniformType::Time: {
//                        auto uniform_time = static_cast<UniformTime&>(*uniforms[uniform->getName()]);
//
//                        if (uniform->isChanged() || force) {
//                            Texture::copy(uniforms[name], index, texture);
//                        }
                    }
                        break;

                    case UniformType::ValueArray:
                        throw material_exception{"batching of uniform arrays is not supported yet!"};
                }
            }
        }
    };
}