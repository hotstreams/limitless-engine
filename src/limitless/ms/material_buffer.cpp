#include <limitless/ms/material_buffer.hpp>

#include <limitless/core/uniform/uniform.hpp>
#include <limitless/core/uniform/uniform_value.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/uniform/uniform_time.hpp>
#include <limitless/core/context_initializer.hpp>

#include <glm/glm.hpp>
#include <cstring>
#include <limitless/core/texture/bindless_texture.hpp>
#include <limitless/ms/material.hpp>

using namespace Limitless;
using namespace Limitless::ms;

template<typename V>
void MaterialBuffer::map(std::vector<std::byte>& block, const Uniform& uniform) const {
    const auto& uni = static_cast<const UniformValue<V>&>(uniform);
    const auto offset = uniform_offsets.at(uniform.getName());
    std::memcpy(block.data() + offset, &uni.getValue(), sizeof(V));
}

void MaterialBuffer::map(std::vector<std::byte>& block, Uniform& uniform) {
    switch (uniform.getType()) {
        case UniformType::Value:
            switch (uniform.getValueType()) {
                case UniformValueType::Uint:
                    map<unsigned int>(block, uniform);
                    break;
                case UniformValueType::Int:
                    map<int>(block, uniform);
                    break;
                case UniformValueType::Float:
                    map<float>(block, uniform);
                    break;
                case UniformValueType::Vec2:
                    map<glm::vec2>(block, uniform);
                    break;
                case UniformValueType::Vec3:
                    map<glm::vec3>(block, uniform);
                    break;
                case UniformValueType::Vec4:
                    map<glm::vec4>(block, uniform);
                    break;
                case UniformValueType::Mat4:
                    map<glm::mat4>(block, uniform);
                    break;
                case UniformValueType::Mat3:
                    map<glm::mat3>(block, uniform);
                    break;
            }
            break;
        case UniformType::Sampler:
            if (ContextInitializer::isBindlessTextureSupported()) {
                const auto& uni = static_cast<const UniformSampler&>(uniform); //NOLINT
                const auto offset = uniform_offsets.at(uniform.getName());
                auto& bindless_texture = static_cast<BindlessTexture&>(uni.getSampler()->getExtensionTexture()); //NOLINT
                bindless_texture.makeResident();
                std::memcpy(block.data() + offset, &bindless_texture.getHandle(), sizeof(uint64_t));
            }
            break;
        case UniformType::Time: {
            auto& time = static_cast<UniformTime&>(uniform); //NOLINT
            time.update();
            map<float>(block, uniform);
            break;
        }
    }
}

void MaterialBuffer::map(const Material& material) {
    std::vector<std::byte> block(material_buffer->getSize());

    for (const auto& [_, uniform] : material.getProperties()) {
        map(block, *uniform);
    }

    for (const auto& [_, uniform] : material.getUniforms()) {
        map(block, *uniform);
    }

    // adding shading in the end
    auto shading = material.getShading();
    std::memcpy(block.data() + block.size() - sizeof(uint32_t), &shading, sizeof(uint32_t));

    material_buffer->mapData(block.data(), block.size());
}