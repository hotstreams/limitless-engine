#pragma once

#include <memory>

namespace LimitlessEngine {
    class Uniform;
    class ByteBuffer;
    enum class UniformValueType;

    class UniformSerializer {
    private:
        void serializeUniform(const Uniform& uniform, ByteBuffer& buffer);
        void serializeUniformValue(const Uniform& uniform, ByteBuffer& buffer);
        void serializeUniformSampler(const Uniform& uniform, ByteBuffer& buffer);
        template<typename T>
        void serializeUniformValue(const Uniform& uniform, ByteBuffer& buffer);

        Uniform* deserializeUniformValue(ByteBuffer& buffer, std::string&& name, UniformValueType value_type);
        Uniform* deserializeUniformSampler(ByteBuffer& buffer, std::string&& name);
        Uniform* deserializeUniformTime(ByteBuffer& buffer, std::string&& name);
        template<typename T>
        Uniform* deserializeUniformValue(ByteBuffer& buffer, std::string&& name);
    public:
        ByteBuffer serialize(const Uniform& uniform);
        std::unique_ptr<Uniform> deserialize(ByteBuffer& buffer);
    };

    ByteBuffer& operator<<(ByteBuffer& buffer, const Uniform& uniform);
    ByteBuffer& operator>>(ByteBuffer& buffer, std::unique_ptr<Uniform>& uniform);
}