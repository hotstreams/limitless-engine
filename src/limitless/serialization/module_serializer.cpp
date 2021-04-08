#include <limitless/serialization/module_serializer.hpp>

#include <limitless/serialization/distribution_serializer.hpp>
#include <limitless/particle_system/emitter.hpp>
#include <limitless/assets.hpp>
#include <memory>

using namespace LimitlessEngine;

ByteBuffer ModuleSerializer::serialize(EmitterModule& module) {
    ByteBuffer buffer;

    buffer << module.getType();

    switch (module.getType()) {
        case EmitterModuleType::InitialLocation:
            buffer << static_cast<InitialLocation&>(module).getDistribution();
            break;
        case EmitterModuleType::InitialRotation:
            buffer << static_cast<InitialRotation&>(module).getDistribution();
            break;
        case EmitterModuleType::InitialVelocity:
            buffer << static_cast<InitialVelocity&>(module).getDistribution();
            break;
        case EmitterModuleType::InitialColor:
            buffer << static_cast<InitialColor&>(module).getDistribution();
            break;
        case EmitterModuleType::InitialSize:
            buffer << static_cast<InitialSize&>(module).getDistribution();
            break;
        case EmitterModuleType::InitialAcceleration:
            buffer << static_cast<InitialAcceleration&>(module).getDistribution();
            break;
        case EmitterModuleType::MeshLocation:
            buffer << static_cast<MeshLocation&>(module).getMesh()->getName();
            break;
        case EmitterModuleType::SubUV:
            buffer << static_cast<SubUV&>(module).getTextureSize()
                   << static_cast<SubUV&>(module).getFPS()
                   << static_cast<SubUV&>(module).getFrameCount();
            break;
        case EmitterModuleType::ColorByLife:
            buffer << static_cast<ColorByLife&>(module).getDistribution();
            break;
        case EmitterModuleType::RotationRate:
            buffer << static_cast<RotationRate&>(module).getDistribution();
            break;
        case EmitterModuleType::SizeByLife:
            buffer << static_cast<SizeByLife&>(module).getDistribution()
                   << static_cast<SizeByLife&>(module).getFactor();
            break;
        case EmitterModuleType::CustomMaterial: {
            auto& props = static_cast<CustomMaterialModule&>(module).getProperties();
            size_t count = std::count_if(props.begin(), props.end(), [] (const auto& distr) { return distr != nullptr; });
            buffer << count;
            for (uint32_t i = 0; i < props.size(); ++i) {
                if (props[i]) {
                    buffer << i << props[i];
                }
            }
            break;
        }
        case EmitterModuleType::CustomMaterialOverLife: {
            auto& props = static_cast<CustomMaterialModuleOverLife&>(module).getProperties();
            size_t count = std::count_if(props.begin(), props.end(), [] (const auto& distr) { return distr != nullptr; });
            buffer << count;
            for (uint32_t i = 0; i < props.size(); ++i) {
                if (props[i]) {
                    buffer << i << props[i];
                }
            }
            break;
        }
        case EmitterModuleType::Lifetime:
            buffer << static_cast<Lifetime&>(module).getDistribution();
            break;
    }

    return buffer;
}

std::unique_ptr<EmitterModule> ModuleSerializer::deserialize(ByteBuffer& buffer, Assets& assets) {
    EmitterModuleType type{};
    buffer >> type;

    std::unique_ptr<EmitterModule> module;
    switch (type) {
        case EmitterModuleType::InitialLocation: {
            std::unique_ptr<Distribution<glm::vec3>> distr;
            buffer >> distr;
            module = std::unique_ptr<EmitterModule>(new InitialLocation(distr.release()));
            break;
        }
        case EmitterModuleType::InitialRotation: {
            std::unique_ptr<Distribution<glm::vec3>> distr;
            buffer >> distr;
            module = std::unique_ptr<EmitterModule>(new InitialRotation(distr.release()));
            break;
        }
        case EmitterModuleType::InitialVelocity: {
            std::unique_ptr<Distribution<glm::vec3>> distr;
            buffer >> distr;
            module = std::unique_ptr<EmitterModule>(new InitialVelocity(distr.release()));
            break;
        }
        case EmitterModuleType::InitialColor: {
            std::unique_ptr<Distribution<glm::vec4>> distr;
            buffer >> distr;
            module = std::unique_ptr<EmitterModule>(new InitialColor(distr.release()));
            break;
        }
        case EmitterModuleType::InitialSize: {
            std::unique_ptr<Distribution<float>> distr;
            buffer >> distr;
            module = std::unique_ptr<EmitterModule>(new InitialSize(distr.release()));
            break;
        }
        case EmitterModuleType::InitialAcceleration: {
            std::unique_ptr<Distribution<glm::vec3>> distr;
            buffer >> distr;
            module = std::unique_ptr<EmitterModule>(new InitialAcceleration(distr.release()));
            break;
        }
        case EmitterModuleType::MeshLocation: {
            std::string mesh_name;
            buffer >> mesh_name;
            module = std::make_unique<MeshLocation>(assets.meshes.at(mesh_name));
            break;
        }
        case EmitterModuleType::SubUV: {
            glm::vec2 texture_size;
            glm::vec2 frame_count;
            float fps;
            buffer >> texture_size >> fps >> frame_count;
            module = std::unique_ptr<EmitterModule>(new SubUV(texture_size, fps, frame_count));
            break;
        }
        case EmitterModuleType::ColorByLife: {
            std::unique_ptr<Distribution<glm::vec4>> distr;
            buffer >> distr;
            module = std::unique_ptr<EmitterModule>(new ColorByLife(distr.release()));
            break;
        }
        case EmitterModuleType::RotationRate: {
            std::unique_ptr<Distribution<glm::vec3>> distr;
            buffer >> distr;
            module = std::unique_ptr<EmitterModule>(new RotationRate(distr.release()));
            break;
        }
        case EmitterModuleType::SizeByLife: {
            std::unique_ptr<Distribution<float>> distr;
            float factor;
            buffer >> distr >> factor;
            module = std::unique_ptr<EmitterModule>(new SizeByLife(distr.release(), factor));
            break;
        }
        case EmitterModuleType::CustomMaterial: {
            std::array<std::unique_ptr<Distribution<float>>, 4> properties {};
            size_t count {};
            buffer >> count;
            for (uint32_t i = 0; i < count; ++i) {
                uint32_t index {};
                buffer >> index;
                buffer >> properties[index];
            }

            module = std::unique_ptr<EmitterModule>(new CustomMaterialModule(properties[0].release(),
                                                                             properties[1].release(),
                                                                             properties[2].release(),
                                                                             properties[3].release()));
            break;
        }
        case EmitterModuleType::CustomMaterialOverLife: {
            std::array<std::unique_ptr<Distribution<float>>, 4> properties {};
            size_t count {};
            buffer >> count;
            for (uint32_t i = 0; i < count; ++i) {
                uint32_t index {};
                buffer >> index;
                buffer >> properties[index];
            }

            module = std::unique_ptr<EmitterModule>(new CustomMaterialModuleOverLife(properties[0].release(),
                                                                                     properties[1].release(),
                                                                                     properties[2].release(),
                                                                                     properties[3].release()));
            break;
        }
        case EmitterModuleType::Lifetime: {
            std::unique_ptr<Distribution<float>> distr;
            buffer >> distr;
            module = std::unique_ptr<EmitterModule>(new Lifetime(distr.release()));
            break;
        }
    }

    return module;
}

ByteBuffer& LimitlessEngine::operator<<(ByteBuffer& buffer, EmitterModule& module) {
    ModuleSerializer serializer;
    buffer << serializer.serialize(module);
    return buffer;
}

ByteBuffer& LimitlessEngine::operator>>(ByteBuffer& buffer, const AssetDeserializer<std::unique_ptr<EmitterModule>>& asset) {
    ModuleSerializer serializer;
    auto& [context, assets, module] = asset;
    module = serializer.deserialize(buffer, assets);
    return buffer;
}
