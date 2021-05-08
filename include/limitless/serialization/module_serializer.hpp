#pragma once

#include <limitless/serialization/asset_deserializer.hpp>
#include <limitless/fx/modules/initial_location.hpp>
#include <limitless/fx/modules/initial_rotation.hpp>
#include <limitless/fx/modules/initial_velocity.hpp>
#include <limitless/fx/modules/initial_color.hpp>
#include <limitless/fx/modules/initial_size.hpp>
#include <limitless/fx/modules/initial_acceleration.hpp>
#include <limitless/fx/modules/subuv.hpp>
#include <limitless/fx/modules/color_by_life.hpp>
#include <limitless/fx/modules/rotation_rate.hpp>
#include <limitless/fx/modules/size_by_life.hpp>
#include <limitless/fx/modules/custom_material.hpp>
#include <limitless/fx/modules/custom_material_by_life.hpp>
#include <limitless/fx/modules/lifetime.hpp>
#include <limitless/fx/modules/velocity_by_life.hpp>
#include <limitless/fx/modules/distribution.hpp>
#include <limitless/fx/modules/beam_builder.hpp>
#include <limitless/fx/modules/beam_target.hpp>
#include <limitless/fx/modules/beam_rebuild.hpp>
#include <limitless/fx/modules/beam_offset.hpp>
#include <limitless/fx/modules/beam_displacement.hpp>

#include <limitless/serialization/distribution_serializer.hpp>

#include <limitless/util/bytebuffer.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class ModuleSerializer {
    public:
        ByteBuffer serialize(fx::Module<Particle>& module) {
            ByteBuffer buffer;

            buffer << module.getType();

            switch (module.getType()) {
                case ModuleType::InitialLocation:
                    buffer << static_cast<InitialLocation<Particle>&>(module).getDistribution();
                    break;
                case ModuleType::InitialRotation:
                    buffer << static_cast<InitialRotation<Particle>&>(module).getDistribution();
                    break;
                case ModuleType::InitialVelocity:
                    buffer << static_cast<InitialVelocity<Particle>&>(module).getDistribution();
                    break;
                case ModuleType::InitialColor:
                    buffer << static_cast<InitialColor<Particle>&>(module).getDistribution();
                    break;
                case ModuleType::InitialSize:
                    buffer << static_cast<InitialSize<Particle>&>(module).getDistribution();
                    break;
                case ModuleType::InitialAcceleration:
                    buffer << static_cast<InitialAcceleration<Particle>&>(module).getDistribution();
                    break;
//                case ModuleType::MeshLocation:
//                    buffer << static_cast<MeshLocation<Particle>&>(module).getMesh()->getName();
//                    break;
                case ModuleType::SubUV:
                    buffer << static_cast<SubUV<Particle>&>(module).getTextureSize()
                           << static_cast<SubUV<Particle>&>(module).getFPS()
                           << static_cast<SubUV<Particle>&>(module).getFrameCount();
                    break;
                case ModuleType::ColorByLife:
                    buffer << static_cast<ColorByLife<Particle>&>(module).getDistribution();
                    break;
                case ModuleType::RotationRate:
                    buffer << static_cast<RotationRate<Particle>&>(module).getDistribution();
                    break;
                case ModuleType::SizeByLife:
                    buffer << static_cast<SizeByLife<Particle>&>(module).getDistribution()
                           << static_cast<SizeByLife<Particle>&>(module).getFactor();
                    break;
                case ModuleType::CustomMaterial: {
                    auto& props = static_cast<CustomMaterial<Particle>&>(module).getProperties();
                    size_t count = std::count_if(props.begin(), props.end(), [] (const auto& distr) { return distr != nullptr; });
                    buffer << count;
                    for (uint32_t i = 0; i < props.size(); ++i) {
                        if (props[i]) {
                            buffer << i << props[i];
                        }
                    }
                    break;
                }
                case ModuleType::CustomMaterialByLife: {
                    auto& props = static_cast<CustomMaterialByLife<Particle>&>(module).getProperties();
                    size_t count = std::count_if(props.begin(), props.end(), [] (const auto& distr) { return distr != nullptr; });
                    buffer << count;
                    for (uint32_t i = 0; i < props.size(); ++i) {
                        if (props[i]) {
                            buffer << i << props[i];
                        }
                    }
                    break;
                }
                case ModuleType::Lifetime: {
                    buffer << static_cast<Lifetime<Particle>&>(module).getDistribution();
                    break;
                }
                case ModuleType::VelocityByLife: {
                    buffer << static_cast<VelocityByLife<Particle>&>(module).getDistribution();
                    break;
                }
                case ModuleType::Beam_InitialDisplacement: {
                    if constexpr (std::is_same_v<Particle, BeamParticle>) {
                        buffer << static_cast<BeamDisplacement<Particle>&>(module).getDistribution();
                    }
                    break;
                }
                case ModuleType::Beam_InitialOffset: {
                    if constexpr (std::is_same_v<Particle, BeamParticle>) {
                        buffer << static_cast<BeamOffset<Particle>&>(module).getDistribution();
                    }
                    break;
                }
                case ModuleType::Beam_InitialRebuild: {
                    if constexpr (std::is_same_v<Particle, BeamParticle>) {
                        buffer << static_cast<BeamRebuild<Particle>&>(module).getDistribution();
                    }
                    break;
                }
                case ModuleType::Beam_InitialTarget: {
                    if constexpr (std::is_same_v<Particle, BeamParticle>) {
                        buffer << static_cast<BeamTarget<Particle>&>(module).getDistribution();
                    }
                    break;
                }
                case ModuleType::BeamBuilder: {
                    break;
                }
            }

            return buffer;
        }

        std::unique_ptr<fx::Module<Particle>> deserialize(ByteBuffer& buffer, [[maybe_unused]] Assets& assets) {
            ModuleType type{};
            buffer >> type;

            std::unique_ptr<Module<Particle>> module;
            switch (type) {
                case ModuleType::InitialLocation: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<InitialLocation<Particle>>(std::move(distr));
                    break;
                }
                case ModuleType::InitialRotation: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<InitialRotation<Particle>>(std::move(distr));
                    break;
                }
                case ModuleType::InitialVelocity: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<InitialVelocity<Particle>>(std::move(distr));
                    break;
                }
                case ModuleType::InitialColor: {
                    std::unique_ptr<Distribution<glm::vec4>> distr;
                    buffer >> distr;
                    module = std::make_unique<InitialColor<Particle>>(std::move(distr));
                    break;
                }
                case ModuleType::InitialSize: {
                    std::unique_ptr<Distribution<float>> distr;
                    buffer >> distr;
                    module = std::make_unique<InitialSize<Particle>>(std::move(distr));
                    break;
                }
                case ModuleType::InitialAcceleration: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<InitialAcceleration<Particle>>(std::move(distr));
                    break;
                }
//                case ModuleType::MeshLocation: {
//                    std::string mesh_name;
//                    buffer >> mesh_name;
//                    module = std::make_unique<MeshLocation>(assets.meshes.at(mesh_name));
//                    break;
//                }
                case ModuleType::SubUV: {
                    glm::vec2 texture_size;
                    glm::vec2 frame_count;
                    float fps;
                    buffer >> texture_size >> fps >> frame_count;
                    module = std::make_unique<SubUV<Particle>>(texture_size, fps, frame_count);
                    break;
                }
                case ModuleType::ColorByLife: {
                    std::unique_ptr<Distribution<glm::vec4>> distr;
                    buffer >> distr;
                    module = std::make_unique<ColorByLife<Particle>>(std::move(distr));
                    break;
                }
                case ModuleType::RotationRate: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<RotationRate<Particle>>(std::move(distr));
                    break;
                }
                case ModuleType::SizeByLife: {
                    std::unique_ptr<Distribution<float>> distr;
                    float factor;
                    buffer >> distr >> factor;
                    module = std::make_unique<SizeByLife<Particle>>(std::move(distr), factor);
                    break;
                }
                case ModuleType::CustomMaterial: {
                    std::array<std::unique_ptr<Distribution<float>>, 4> properties {};
                    size_t count {};
                    buffer >> count;
                    for (uint32_t i = 0; i < count; ++i) {
                        uint32_t index {};
                        buffer >> index;
                        buffer >> properties[index];
                    }

                    module = std::make_unique<CustomMaterial<Particle>>(std::move(properties[0]), std::move(properties[1]), std::move(properties[2]), std::move(properties[3]));
                    break;
                }
                case ModuleType::CustomMaterialByLife: {
                    std::array<std::unique_ptr<Distribution<float>>, 4> properties {};
                    size_t count {};
                    buffer >> count;
                    for (uint32_t i = 0; i < count; ++i) {
                        uint32_t index {};
                        buffer >> index;
                        buffer >> properties[index];
                    }

                    module = std::make_unique<CustomMaterialByLife<Particle>>(std::move(properties[0]), std::move(properties[1]), std::move(properties[2]), std::move(properties[3]));
                    break;
                }
                case ModuleType::Lifetime: {
                    std::unique_ptr<Distribution<float>> distr;
                    buffer >> distr;
                    module = std::make_unique<Lifetime<Particle>>(std::move(distr));
                    break;
                }
                case ModuleType::VelocityByLife: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<VelocityByLife<Particle>>(std::move(distr));
                    break;
                }
                case ModuleType::Beam_InitialDisplacement: {
                    if constexpr (std::is_same_v<Particle, BeamParticle>) {
                        std::unique_ptr<Distribution<float>> distr;
                        buffer >> distr;
                        module = std::make_unique<BeamDisplacement<Particle>>(std::move(distr));
                    }
                    break;
                }
                case ModuleType::Beam_InitialOffset: {
                    if constexpr (std::is_same_v<Particle, BeamParticle>) {
                        std::unique_ptr<Distribution<float>> distr;
                        buffer >> distr;
                        module = std::make_unique<BeamOffset<Particle>>(std::move(distr));
                    }
                    break;
                }
                case ModuleType::Beam_InitialRebuild: {
                    if constexpr (std::is_same_v<Particle, BeamParticle>) {
                        std::unique_ptr<Distribution<float>> distr;
                        buffer >> distr;
                        module = std::make_unique<BeamRebuild<Particle>>(std::move(distr));
                    }
                    break;
                }
                case ModuleType::Beam_InitialTarget: {
                    if constexpr (std::is_same_v<Particle, BeamParticle>) {
                        std::unique_ptr<Distribution<glm::vec3>> distr;
                        buffer >> distr;
                        module = std::make_unique<BeamTarget<Particle>>(std::move(distr));
                    }
                    break;
                }
                case ModuleType::BeamBuilder: {
                    if constexpr (std::is_same_v<Particle, BeamParticle>) {
                        module = std::make_unique<BeamBuilder<Particle>>();
                    }
                    break;
                }
            }

            return module;
        }
    };

    template<typename Particle>
    ByteBuffer& operator<<(ByteBuffer& buffer, fx::Module<Particle>& module) {
        ModuleSerializer<Particle> serializer;
        buffer << serializer.serialize(module);
        return buffer;
    }

    template<typename Particle>
    ByteBuffer& operator>>(ByteBuffer& buffer, const AssetDeserializer<std::unique_ptr<fx::Module<Particle>>>& asset) {
        ModuleSerializer<Particle> serializer;
        auto& [context, assets, settings, module] = asset;
        module = serializer.deserialize(buffer, assets);
        return buffer;
    }
}