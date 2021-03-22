#pragma once

#include <memory>

namespace LimitlessEngine {
    class ByteBuffer;
    class EmitterModule;

    class ModuleSerializer {
    public:
        ByteBuffer serialize(EmitterModule& module);
        std::unique_ptr<EmitterModule> deserialize(ByteBuffer& buffer);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, EmitterModule& module);
    ByteBuffer& operator>>(ByteBuffer& buffer, std::unique_ptr<EmitterModule>& module);
}