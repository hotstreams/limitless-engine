#pragma once

#include <memory>
#include <limitless/serialization/asset_deserializer.hpp>
#include <limitless/core/uniform/uniform_value_type.hpp>

namespace Limitless {
    class Uniform;
    class ByteBuffer;
    class Assets;

    class UniformSerializer {
    private:
        static constexpr uint8_t VERSION = 0x1;

        void serializeUniform(const Uniform& uniform, ByteBuffer& buffer);
        void serializeUniformValue(const Uniform& uniform, ByteBuffer& buffer);
        void serializeUniformSampler(const Uniform& uniform, ByteBuffer& buffer);
        template<typename T>
        void serializeUniformValue(const Uniform& uniform, ByteBuffer& buffer);

        Uniform* deserializeUniformValue(ByteBuffer& buffer, std::string&& name, UniformValueType value_type);
        Uniform* deserializeUniformSampler(ByteBuffer& buffer, Assets& assets, std::string&& name);
        Uniform* deserializeUniformTime(ByteBuffer& buffer, std::string&& name);
        template<typename T>
        Uniform* deserializeUniformValue(ByteBuffer& buffer, std::string&& name);
    public:
        ByteBuffer serialize(const Uniform& uniform);
        std::unique_ptr<Uniform> deserialize(ByteBuffer& buffer, Assets& assets);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const Uniform& uniform);
    ByteBuffer& operator>>(ByteBuffer& buffer, const AssetDeserializer<std::unique_ptr<Uniform>>& asset);
}