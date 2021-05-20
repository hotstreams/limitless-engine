#include <limitless/serialization/emitter_serializer.hpp>

#include <limitless/serialization/module_serializer.hpp>
#include <limitless/serialization/distribution_serializer.hpp>
#include <limitless/serialization/material_serializer.hpp>

#include <limitless/fx/effect_builder.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>
#include <limitless/models/abstract_mesh.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>

using namespace Limitless;
using namespace Limitless::fx;

ByteBuffer EmitterSerializer::serialize(const AbstractEmitter& emitter) {
    ByteBuffer buffer;

    buffer << emitter.getType()
           << emitter.getLocalPosition()
           << emitter.getLocalRotation()
           << emitter.getLocalSpace()
           << emitter.getSpawn()
           << emitter.getDuration().count();

    switch (emitter.getType()) {
        case AbstractEmitter::Type::Sprite: {
            const auto& sprite_emitter = static_cast<const SpriteEmitter&>(emitter);
            buffer << *sprite_emitter.material
                   << sprite_emitter.modules;
            break;
        }
        case AbstractEmitter::Type::Mesh: {
            const auto& mesh_emitter = static_cast<const MeshEmitter&>(emitter);
            buffer << *mesh_emitter.material
                   << mesh_emitter.modules
                   << mesh_emitter.getMesh()->getName();
            break;
        }
        case AbstractEmitter::Type::Beam: {
            const auto& beam_emitter = static_cast<const BeamEmitter&>(emitter);
            buffer << *beam_emitter.material
                   << beam_emitter.modules;
            break;
        }
    }

    return buffer;
}

void EmitterSerializer::deserialize(Context& context, Assets& assets, const RenderSettings& settings, ByteBuffer& buffer, EffectBuilder& builder) {
    std::string name;
    AbstractEmitter::Type type;
    glm::vec3 local_position;
    glm::quat local_rotation;
    bool local_space;
    EmitterSpawn spawn;
    float duration;
    std::shared_ptr<ms::Material> material;

    buffer >> name
           >> type
           >> local_position
           >> local_rotation
           >> local_space
           >> spawn
           >> duration
           >> AssetDeserializer<std::shared_ptr<ms::Material>>{context, assets, settings, material};

    switch (type) {
        case AbstractEmitter::Type::Sprite: {
            decltype(SpriteEmitter::modules) modules;
            buffer >> AssetDeserializer<decltype(modules)>{context, assets, settings, modules};

            builder.createEmitter<SpriteEmitter>(name)
                   .setModules<SpriteEmitter>(std::move(modules));
            break;
        }
        case AbstractEmitter::Type::Mesh: {
            decltype(MeshEmitter::modules) modules;
            buffer >> AssetDeserializer<decltype(modules)>{context, assets, settings, modules};

            builder.createEmitter<MeshEmitter>(name)
                   .setModules<MeshEmitter>(std::move(modules));

            std::string mesh_name;
            buffer >> mesh_name;

            builder.setMesh(assets.meshes.at(mesh_name));
            break;
        }
        case AbstractEmitter::Type::Beam: {
            decltype(BeamEmitter::modules) modules;
            buffer >> AssetDeserializer<decltype(modules)>{context, assets, settings, modules};

            builder.createEmitter<BeamEmitter>(name)
                   .setModules<BeamEmitter>(std::move(modules));
            break;
        }
    }

    builder .setLocalPosition(local_position)
            .setLocalRotation(local_rotation)
            .setLocalSpace(local_space)
            .setSpawn(std::move(spawn))
            .setDuration(std::chrono::duration<float>{duration})
            .setMaterial(material);
}

ByteBuffer& Limitless::operator<<(ByteBuffer& buffer, const EmitterSpawn& spawn) {
    buffer << spawn.mode
           << spawn.max_count
           << spawn.spawn_rate;

    if (spawn.mode == EmitterSpawn::Mode::Burst) {
        buffer << spawn.burst->burst_count
               << spawn.burst->loops
               << spawn.burst->loops_done;
    }

    return buffer;
}

ByteBuffer& Limitless::operator>>(ByteBuffer& buffer, EmitterSpawn& spawn) {
    buffer >> spawn.mode
           >> spawn.max_count
           >> spawn.spawn_rate;

    if (spawn.mode == EmitterSpawn::Mode::Burst) {
        spawn.burst = EmitterSpawn::Burst{};
        buffer >> spawn.burst->burst_count
               >> spawn.burst->loops
               >> spawn.burst->loops_done;
    }

    return buffer;
}

ByteBuffer& Limitless::operator<<(ByteBuffer& buffer, const AbstractEmitter& emitter) {
    EmitterSerializer serializer;
    buffer << serializer.serialize(emitter);
    return buffer;
}
