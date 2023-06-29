#include <limitless/serialization/uniform_serializer.hpp>

#include <limitless/core/uniform/uniform.hpp>
#include <limitless/util/bytebuffer.hpp>
#include <limitless/core/texture.hpp>
#include <glm/glm.hpp>
#include <limitless/loaders/texture_loader.hpp>
#include <limitless/assets.hpp>

#include <limitless/core/uniform/uniform_value.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/uniform/uniform_time.hpp>

using namespace Limitless;

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

Uniform* UniformSerializer::deserializeUniformSampler(ByteBuffer& buffer, Assets& assets, std::string&& name) {
    std::string p;
    buffer >> p;

    auto path = convertPathSeparators(p);

    std::shared_ptr<Texture> texture {};
    try {
        texture = assets.textures.at(path.string());
    } catch (...) {
        texture = TextureLoader::load(assets, path);
    }

    return new UniformSampler(name, std::move(texture));
}

Uniform* UniformSerializer::deserializeUniformTime(ByteBuffer& buffer, std::string&& name) {
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
    serializeUniform(uniform, buffer);

    buffer << static_cast<const UniformSampler&>(uniform).sampler->getPath().value().string();
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

    buffer << VERSION;

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

std::unique_ptr<Uniform> UniformSerializer::deserialize(ByteBuffer& buffer, Assets& assets) {
    uint8_t version {};

    buffer >> version;

    if (version != VERSION) {
        throw std::runtime_error("Wrong uniform serializer version! " + std::to_string(VERSION) + " vs " + std::to_string(version));
    }

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
            uniform = deserializeUniformSampler(buffer, assets, std::move(name));
            break;
    }

    return std::unique_ptr<Uniform>(uniform);
}

ByteBuffer& Limitless::operator<<(ByteBuffer& buffer, const Uniform& uniform) {
    UniformSerializer serializer;
    buffer << serializer.serialize(uniform);
    return buffer;
}

ByteBuffer& Limitless::operator>>(ByteBuffer& buffer, const AssetDeserializer<std::unique_ptr<Uniform>>& asset) {
    UniformSerializer serializer;
    auto& [assets, uniform] = asset;
    uniform = serializer.deserialize(buffer, assets);
    return buffer;
}

