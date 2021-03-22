#pragma once

#include <limitless/serialization/asset_deserializer.hpp>
#include <memory>

namespace LimitlessEngine {
    class ByteBuffer;
    class Assets;
    class EmitterModule;

    class ModuleSerializer {
    public:
        ByteBuffer serialize(EmitterModule& module);
        std::unique_ptr<EmitterModule> deserialize(ByteBuffer& buffer, Assets& assets);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, EmitterModule& module);
    ByteBuffer& operator>>(ByteBuffer& buffer, const AssetDeserializer<std::unique_ptr<EmitterModule>>& module);
}