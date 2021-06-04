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
#include <limitless/fx/modules/mesh_location.hpp>

#include <limitless/serialization/distribution_serializer.hpp>

#include <limitless/util/bytebuffer.hpp>
#include <limitless/assets.hpp>
#include <limitless/fx/modules/mesh_location_attachment.hpp>
#include <limitless/fx/modules/time.hpp>

namespace Limitless {
    template<typename Particle>
    class ModuleSerializer {
    private:
        static constexpr uint8_t VERSION = 0x4;
    public:
        ByteBuffer serialize(const fx::Module<Particle>& module) {
            ByteBuffer buffer;

            buffer << VERSION;

            buffer << module.getType();

            switch (module.getType()) {
                case fx::ModuleType::InitialLocation:
                    buffer << static_cast<const fx::InitialLocation<Particle>&>(module).getDistribution();
                    break;
                case fx::ModuleType::InitialRotation:
                    buffer << static_cast<const fx::InitialRotation<Particle>&>(module).getDistribution();
                    break;
                case fx::ModuleType::InitialVelocity:
                    buffer << static_cast<const fx::InitialVelocity<Particle>&>(module).getDistribution();
                    break;
                case fx::ModuleType::InitialColor:
                    buffer << static_cast<const fx::InitialColor<Particle>&>(module).getDistribution();
                    break;
                case fx::ModuleType::InitialSize:
                    buffer << static_cast<const fx::InitialSize<Particle>&>(module).getDistribution();
                    break;
                case fx::ModuleType::InitialAcceleration:
                    buffer << static_cast<const fx::InitialAcceleration<Particle>&>(module).getDistribution();
                    break;
                case fx::ModuleType::MeshLocationAttachment: {
                    if (std::holds_alternative<std::shared_ptr<AbstractModel>>(static_cast<const fx::MeshLocationAttachment<Particle>&>(module).getMesh())) {
                        buffer << std::get<std::shared_ptr<AbstractModel>>(static_cast<const fx::MeshLocationAttachment<Particle>&>(module).getMesh())->getName();
                    } else {
                        buffer << std::get<std::shared_ptr<AbstractMesh>>(static_cast<const fx::MeshLocationAttachment<Particle>&>(module).getMesh())->getName();
                    }
                    break;
                }
                case fx::ModuleType::InitialMeshLocation: {
                    if (std::holds_alternative<std::shared_ptr<AbstractModel>>(static_cast<const fx::InitialMeshLocation<Particle>&>(module).getMesh())) {
                        buffer << std::get<std::shared_ptr<AbstractModel>>(static_cast<const fx::InitialMeshLocation<Particle>&>(module).getMesh())->getName();
                    } else {
                        buffer << std::get<std::shared_ptr<AbstractMesh>>(static_cast<const fx::InitialMeshLocation<Particle>&>(module).getMesh())->getName();
                    }
                    break;
                }
                case fx::ModuleType::SubUV:
                    buffer << static_cast<const fx::SubUV<Particle>&>(module).getTextureSize()
                           << static_cast<const fx::SubUV<Particle>&>(module).getFPS()
                           << static_cast<const fx::SubUV<Particle>&>(module).getFrameCount();
                    break;
                case fx::ModuleType::ColorByLife:
                    buffer << static_cast<const fx::ColorByLife<Particle>&>(module).getDistribution();
                    break;
                case fx::ModuleType::RotationRate:
                    buffer << static_cast<const fx::RotationRate<Particle>&>(module).getDistribution();
                    break;
                case fx::ModuleType::SizeByLife:
                    buffer << static_cast<const fx::SizeByLife<Particle>&>(module).getDistribution();
                    break;
                case fx::ModuleType::CustomMaterial: {
                    auto& props = static_cast<const fx::CustomMaterial<Particle>&>(module).getProperties();
                    size_t count = std::count_if(props.begin(), props.end(), [] (const auto& distr) { return distr != nullptr; });
                    buffer << count;
                    for (uint32_t i = 0; i < props.size(); ++i) {
                        if (props[i]) {
                            buffer << i << props[i];
                        }
                    }
                    break;
                }
                case fx::ModuleType::CustomMaterialByLife: {
                    auto& props = static_cast<const fx::CustomMaterialByLife<Particle>&>(module).getProperties();
                    size_t count = std::count_if(props.begin(), props.end(), [] (const auto& distr) { return distr != nullptr; });
                    buffer << count;
                    for (uint32_t i = 0; i < props.size(); ++i) {
                        if (props[i]) {
                            buffer << i << props[i];
                        }
                    }
                    break;
                }
                case fx::ModuleType::Lifetime: {
                    buffer << static_cast<const fx::Lifetime<Particle>&>(module).getDistribution();
                    break;
                }
                case fx::ModuleType::VelocityByLife: {
                    buffer << static_cast<const fx::VelocityByLife<Particle>&>(module).getDistribution();
                    break;
                }
                case fx::ModuleType::Beam_InitialDisplacement: {
                    if constexpr (std::is_same_v<Particle, fx::BeamParticle>) {
                        buffer << static_cast<const fx::BeamDisplacement<Particle>&>(module).getDistribution();
                    }
                    break;
                }
                case fx::ModuleType::Beam_InitialOffset: {
                    if constexpr (std::is_same_v<Particle, fx::BeamParticle>) {
                        buffer << static_cast<const fx::BeamOffset<Particle>&>(module).getDistribution();
                    }
                    break;
                }
                case fx::ModuleType::Beam_InitialRebuild: {
                    if constexpr (std::is_same_v<Particle, fx::BeamParticle>) {
                        buffer << static_cast<const fx::BeamRebuild<Particle>&>(module).getDistribution();
                    }
                    break;
                }
                case fx::ModuleType::Beam_InitialTarget: {
                    if constexpr (std::is_same_v<Particle, fx::BeamParticle>) {
                        buffer << static_cast<const fx::BeamTarget<Particle>&>(module).getDistribution();
                    }
                    break;
                }
                case fx::ModuleType::BeamBuilder: {
                    break;
                }
                case fx::ModuleType::Time: {
                    break;
                }
            }

            return buffer;
        }

        std::unique_ptr<fx::Module<Particle>> deserialize(ByteBuffer& buffer, [[maybe_unused]] Assets& assets) {
            uint8_t version {};

            buffer >> version;

            if (version != VERSION) {
                throw std::runtime_error("Wrong module version! " + std::to_string(VERSION) + " vs " + std::to_string(version));
            }

            fx::ModuleType type{};
            buffer >> type;

            std::unique_ptr<fx::Module<Particle>> module;
            switch (type) {
                case fx::ModuleType::InitialLocation: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<fx::InitialLocation<Particle>>(std::move(distr));
                    break;
                }
                case fx::ModuleType::InitialRotation: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<fx::InitialRotation<Particle>>(std::move(distr));
                    break;
                }
                case fx::ModuleType::InitialVelocity: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<fx::InitialVelocity<Particle>>(std::move(distr));
                    break;
                }
                case fx::ModuleType::InitialColor: {
                    std::unique_ptr<Distribution<glm::vec4>> distr;
                    buffer >> distr;
                    module = std::make_unique<fx::InitialColor<Particle>>(std::move(distr));
                    break;
                }
                case fx::ModuleType::InitialSize: {
                    if constexpr (std::is_same_v<Particle, fx::MeshParticle>) {
                        std::unique_ptr<Distribution<glm::vec3>> distr;
                        buffer >> distr;
                        module = std::make_unique<fx::InitialSize<Particle>>(std::move(distr));
                    } else {
                        std::unique_ptr<Distribution<float>> distr;
                        buffer >> distr;
                        module = std::make_unique<fx::InitialSize<Particle>>(std::move(distr));
                    }
                    break;
                }
                case fx::ModuleType::InitialAcceleration: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<fx::InitialAcceleration<Particle>>(std::move(distr));
                    break;
                }
                case fx::ModuleType::MeshLocationAttachment: {
                    std::string mesh_name;
                    buffer >> mesh_name;
                    try {
                        module = std::make_unique<fx::MeshLocationAttachment<Particle>>(assets.models.at(mesh_name));
                    } catch (...) {
                        module = std::make_unique<fx::MeshLocationAttachment<Particle>>(assets.meshes.at(mesh_name));
                    }
                    break;
                }
                case fx::ModuleType::InitialMeshLocation: {
                    std::string mesh_name;
                    buffer >> mesh_name;
                    try {
                        module = std::make_unique<fx::InitialMeshLocation<Particle>>(assets.models.at(mesh_name));
                    } catch (...) {
                        module = std::make_unique<fx::InitialMeshLocation<Particle>>(assets.meshes.at(mesh_name));
                    }
                    break;
                }
                case fx::ModuleType::SubUV: {
                    glm::vec2 texture_size;
                    glm::vec2 frame_count;
                    float fps;
                    buffer >> texture_size >> fps >> frame_count;
                    module = std::make_unique<fx::SubUV<Particle>>(texture_size, fps, frame_count);
                    break;
                }
                case fx::ModuleType::ColorByLife: {
                    std::unique_ptr<Distribution<glm::vec4>> distr;
                    buffer >> distr;
                    module = std::make_unique<fx::ColorByLife<Particle>>(std::move(distr));
                    break;
                }
                case fx::ModuleType::RotationRate: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<fx::RotationRate<Particle>>(std::move(distr));
                    break;
                }
                case fx::ModuleType::SizeByLife: {
                    if constexpr (std::is_same_v<Particle, fx::MeshParticle>) {
                        std::unique_ptr<Distribution<glm::vec3>> distr;
                        buffer >> distr;
                        module = std::make_unique<fx::SizeByLife<Particle>>(std::move(distr));
                    } else {
                        std::unique_ptr<Distribution<float>> distr;
                        buffer >> distr;
                        module = std::make_unique<fx::SizeByLife<Particle>>(std::move(distr));
                    }
                    break;
                }
                case fx::ModuleType::CustomMaterial: {
                    std::array<std::unique_ptr<Distribution<float>>, 4> properties {};
                    size_t count {};
                    buffer >> count;
                    for (uint32_t i = 0; i < count; ++i) {
                        uint32_t index {};
                        buffer >> index;
                        buffer >> properties[index];
                    }

                    module = std::make_unique<fx::CustomMaterial<Particle>>(std::move(properties[0]), std::move(properties[1]), std::move(properties[2]), std::move(properties[3]));
                    break;
                }
                case fx::ModuleType::CustomMaterialByLife: {
                    std::array<std::unique_ptr<Distribution<float>>, 4> properties {};
                    size_t count {};
                    buffer >> count;
                    for (uint32_t i = 0; i < count; ++i) {
                        uint32_t index {};
                        buffer >> index;
                        buffer >> properties[index];
                    }

                    module = std::make_unique<fx::CustomMaterialByLife<Particle>>(std::move(properties[0]), std::move(properties[1]), std::move(properties[2]), std::move(properties[3]));
                    break;
                }
                case fx::ModuleType::Lifetime: {
                    std::unique_ptr<Distribution<float>> distr;
                    buffer >> distr;
                    module = std::make_unique<fx::Lifetime<Particle>>(std::move(distr));
                    break;
                }
                case fx::ModuleType::VelocityByLife: {
                    std::unique_ptr<Distribution<glm::vec3>> distr;
                    buffer >> distr;
                    module = std::make_unique<fx::VelocityByLife<Particle>>(std::move(distr));
                    break;
                }
                case fx::ModuleType::Beam_InitialDisplacement: {
                    if constexpr (std::is_same_v<Particle, fx::BeamParticle>) {
                        std::unique_ptr<Distribution<float>> distr;
                        buffer >> distr;
                        module = std::make_unique<fx::BeamDisplacement<Particle>>(std::move(distr));
                    }
                    break;
                }
                case fx::ModuleType::Beam_InitialOffset: {
                    if constexpr (std::is_same_v<Particle, fx::BeamParticle>) {
                        std::unique_ptr<Distribution<float>> distr;
                        buffer >> distr;
                        module = std::make_unique<fx::BeamOffset<Particle>>(std::move(distr));
                    }
                    break;
                }
                case fx::ModuleType::Beam_InitialRebuild: {
                    if constexpr (std::is_same_v<Particle, fx::BeamParticle>) {
                        std::unique_ptr<Distribution<float>> distr;
                        buffer >> distr;
                        module = std::make_unique<fx::BeamRebuild<Particle>>(std::move(distr));
                    }
                    break;
                }
                case fx::ModuleType::Beam_InitialTarget: {
                    if constexpr (std::is_same_v<Particle, fx::BeamParticle>) {
                        std::unique_ptr<Distribution<glm::vec3>> distr;
                        buffer >> distr;
                        module = std::make_unique<fx::BeamTarget<Particle>>(std::move(distr));
                    }
                    break;
                }
                case fx::ModuleType::BeamBuilder: {
                    if constexpr (std::is_same_v<Particle, fx::BeamParticle>) {
                        module = std::make_unique<fx::BeamBuilder<Particle>>();
                    }
                    break;
                }
                case fx::ModuleType::Time: {
                    module = std::make_unique<fx::Time<Particle>>();
                    break;
                }
            }

            return module;
        }
    };

    template<typename Particle>
    ByteBuffer& operator<<(ByteBuffer& buffer, const fx::Module<Particle>& module) {
        ModuleSerializer<Particle> serializer;
        buffer << serializer.serialize(module);
        return buffer;
    }

    template<typename Particle>
    ByteBuffer& operator>>(ByteBuffer& buffer, const AssetDeserializer<std::unique_ptr<fx::Module<Particle>>>& asset) {
        ModuleSerializer<Particle> serializer;
        auto& [assets, module] = asset;
        module = serializer.deserialize(buffer, assets);
        return buffer;
    }
}