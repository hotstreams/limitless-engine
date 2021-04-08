#include <limitless/serialization/emitter_serializer.hpp>

#include <limitless/serialization/module_serializer.hpp>
#include <limitless/serialization/distribution_serializer.hpp>
#include <limitless/serialization/material_serializer.hpp>

#include <limitless/particle_system/effect_builder.hpp>
#include <limitless/material_system/material.hpp>
#include <limitless/assets.hpp>

using namespace LimitlessEngine;

ByteBuffer EmitterSerializer::serialize(const Emitter& emitter) {
    ByteBuffer buffer;

    buffer << emitter.type
           << emitter.local_position
           << emitter.local_rotation
           << emitter.local_space
           << emitter.spawn
           << emitter.duration.count()
           << emitter.modules
           << static_cast<const SpriteEmitter&>(emitter).getMaterial();

    if (emitter.type == EmitterType::Mesh) {
        buffer << static_cast<const MeshEmitter&>(emitter).getMesh()->getName();
    }

    return buffer;
}

void EmitterSerializer::deserialize(Context& context, Assets& assets, ByteBuffer& buffer, EffectBuilder& builder) {
    std::string name;
    EmitterType type;
    glm::vec3 local_position;
    glm::vec3 local_rotation;
    bool local_space;
    EmitterSpawn spawn;
    float duration;
    decltype(Emitter::modules) modules;
    std::shared_ptr<Material> material;

    buffer >> name
           >> type
           >> local_position
           >> local_rotation
           >> local_space
           >> spawn
           >> duration
           >> AssetDeserializer<decltype(modules)>{context, assets, modules}
           >> AssetDeserializer<std::shared_ptr<Material>>{context, assets, material};

    switch (type) {
        case EmitterType::Sprite: {
            builder.createEmitter<SpriteEmitter>(name);
            break;
        }
        case EmitterType::Mesh: {
            builder.createEmitter<MeshEmitter>(name);

            std::string mesh_name;
            buffer >> mesh_name;

            builder.setMesh(assets.meshes.at(mesh_name));
            break;
        }
    }

    builder .setLocalPosition(local_position)
            .setLocalRotation(local_rotation)
            .setLocalSpace(local_space)
            .setSpawn(std::move(spawn))
            .setDuration(std::chrono::duration<float>{duration})
            .setMaterial(material)
            .setModules(std::move(modules));
}

ByteBuffer& LimitlessEngine::operator<<(ByteBuffer& buffer, const EmitterSpawn& spawn) {
    buffer << spawn.mode
           << spawn.max_count
           << spawn.spawn_rate
           << spawn.loops;

    if (spawn.mode == EmitterSpawn::Mode::Burst) {
        buffer << spawn.burst_count;
    }

    return buffer;
}

ByteBuffer& LimitlessEngine::operator>>(ByteBuffer& buffer, EmitterSpawn& spawn) {
    buffer >> spawn.mode
           >> spawn.max_count
           >> spawn.spawn_rate
           >> spawn.loops;

    if (spawn.mode == EmitterSpawn::Mode::Burst) {
        buffer >> spawn.burst_count;
    }

    return buffer;
}

ByteBuffer& LimitlessEngine::operator<<(ByteBuffer& buffer, const Emitter& emitter) {
    EmitterSerializer serializer;
    buffer << serializer.serialize(emitter);
    return buffer;
}
