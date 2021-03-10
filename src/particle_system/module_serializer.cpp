#include <particle_system/module_serializer.hpp>

#include <particle_system/distribution_serializer.hpp>
#include <particle_system/emitter.hpp>
#include <util/bytebuffer.hpp>

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
//            buffer << static_cast<InitialAcceleration&>(module).getDistribution();
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
        case EmitterModuleType::CustomMaterial:

            break;
        case EmitterModuleType::CustomMaterialOverLife:
            break;
        case EmitterModuleType::Lifetime:
            buffer << static_cast<Lifetime&>(module).getDistribution();
            break;
    }

    return buffer;
}

std::unique_ptr<EmitterModule> ModuleSerializer::deserialize(ByteBuffer& buffer) {
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
//            todo
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
            break;
        }
        case EmitterModuleType::CustomMaterialOverLife: {
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

ByteBuffer& LimitlessEngine::operator>>(ByteBuffer& buffer, std::unique_ptr<EmitterModule>& module) {
    ModuleSerializer serializer;
    module = serializer.deserialize(buffer);
    return buffer;
}
