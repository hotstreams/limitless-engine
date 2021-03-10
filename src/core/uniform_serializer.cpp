#include <core/uniform_serializer.hpp>

#include <core/uniform.hpp>
#include <util/bytebuffer.hpp>
#include <glm/glm.hpp>

using namespace LimitlessEngine;

void UniformSerializer::serializeUniform(const Uniform& uniform, ByteBuffer& buffer) {
    buffer << uniform.name
           << uniform.type
           << uniform.value_type;
}

Uniform* UniformSerializer::deserializeUniformValue(ByteBuffer& buffer, std::string&& name, UniformValueType value_type) {
    Uniform* uniform {};
    switch (value_type) {
        case UniformValueType::Float:
            uniform = deserializeUniformValue<float>(buffer, std::move(name));
            break;
        case UniformValueType::Int:
            uniform = deserializeUniformValue<int>(buffer, std::move(name));
            break;
        case UniformValueType::Uint:
            uniform = deserializeUniformValue<unsigned int>(buffer, std::move(name));
            break;
        case UniformValueType::Vec2:
            uniform = deserializeUniformValue<glm::vec2>(buffer, std::move(name));
            break;
        case UniformValueType::Vec3:
            uniform = deserializeUniformValue<glm::vec3>(buffer, std::move(name));
            break;
        case UniformValueType::Vec4:
            uniform = deserializeUniformValue<glm::vec4>(buffer, std::move(name));
            break;
        case UniformValueType::Mat3:
            uniform = deserializeUniformValue<glm::mat3>(buffer, std::move(name));
            break;
        case UniformValueType::Mat4:
            uniform = deserializeUniformValue<glm::mat4>(buffer, std::move(name));
            break;
    }
    return uniform;
}

Uniform* UniformSerializer::deserializeUniformSampler(ByteBuffer& buffer, std::string&& name) {
//            std::string path;
//            buffer >> path;

    std::shared_ptr<Texture> texture {};
//            try {
//                texture = assets.textures.at(path);
//            } catch (...) {
//                texture = TextureLoader::load(path);
//            }

    return new UniformSampler(name, std::move(texture));
}

Uniform* UniformSerializer::deserializeUniformTime(ByteBuffer& buffer, std::string&& name) {
    Uniform* uniform{};

    float value{};
    buffer >> value;

    return new UniformTime(name);
}

void UniformSerializer::serializeUniformValue(const Uniform& uniform, ByteBuffer& buffer) {
    serializeUniform(uniform, buffer);

    switch (uniform.value_type) {
        case UniformValueType::Float:
            serializeUniformValue<float>(uniform, buffer);
            break;
        case UniformValueType::Int:
            serializeUniformValue<int>(uniform, buffer);
            break;
        case UniformValueType::Uint:
            serializeUniformValue<unsigned int>(uniform, buffer);
            break;
        case UniformValueType::Vec2:
            serializeUniformValue<glm::vec2>(uniform, buffer);
            break;
        case UniformValueType::Vec3:
            serializeUniformValue<glm::vec3>(uniform, buffer);
            break;
        case UniformValueType::Vec4:
            serializeUniformValue<glm::vec4>(uniform, buffer);
            break;
        case UniformValueType::Mat3:
            serializeUniformValue<glm::mat3>(uniform, buffer);
            break;
        case UniformValueType::Mat4:
            serializeUniformValue<glm::mat4>(uniform, buffer);
            break;
    }
}

void UniformSerializer::serializeUniformSampler(const Uniform& uniform, ByteBuffer& buffer) {
    //todo
    //buffer << static_cast<UniformSampler&>(uniform).sampler->getPath();
}

template<typename T>
void UniformSerializer::serializeUniformValue(const Uniform& uniform, ByteBuffer& buffer) {
    buffer << static_cast<const UniformValue<T>&>(uniform).value;
}

template<typename T>
Uniform* UniformSerializer::deserializeUniformValue(ByteBuffer& buffer, std::string&& name) {
    T value{};
    buffer >> value;
    return new UniformValue<T>(std::move(name), std::move(value));
}

ByteBuffer UniformSerializer::serialize(const Uniform& uniform) {
    ByteBuffer buffer;
    switch (uniform.type) {
        case UniformType::Value:
        case UniformType::Time:
            serializeUniformValue(uniform, buffer);
            break;
        case UniformType::Sampler:
            serializeUniformSampler(uniform, buffer);
            break;
    }
    return buffer;
}

std::unique_ptr<Uniform> UniformSerializer::deserialize(ByteBuffer& buffer) {
    std::string name{};
    UniformType type{};
    UniformValueType value_type{};

    buffer >> name
           >> type
           >> value_type;

    Uniform* uniform {};
    switch (type) {
        case UniformType::Value:
            uniform = deserializeUniformValue(buffer, std::move(name), value_type);
            break;
        case UniformType::Time:
            uniform = deserializeUniformTime(buffer, std::move(name));
            break;
        case UniformType::Sampler:
            uniform = deserializeUniformSampler(buffer, std::move(name));
            break;
    }

    return std::unique_ptr<Uniform>(uniform);
}

ByteBuffer& LimitlessEngine::operator<<(ByteBuffer& buffer, const Uniform& uniform) {
    UniformSerializer serializer;
    buffer << serializer.serialize(uniform);
    return buffer;
}

ByteBuffer& LimitlessEngine::operator>>(ByteBuffer& buffer, std::unique_ptr<Uniform>& uniform) {
    UniformSerializer serializer;
    uniform = serializer.deserialize(buffer);
    return buffer;
}

