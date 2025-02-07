#include "limitless/core/uniform/uniform_sampler.hpp"

#include <limitless/core/texture/texture_uniform_alignment_getter.hpp>
#include <limitless/core/texture/texture_uniform_size_getter.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/texture/texture.hpp>
#include <limitless/core/uniform/uniform.hpp>
#include <limitless/core/uniform/uniform_value_array.hpp>


using namespace Limitless;

Uniform::Uniform(std::string name, UniformType type, UniformValueType value_type) noexcept
    : name {std::move(name)}
    , type {type}
    , value_type {value_type} {
}

UniformType Uniform::getType() const noexcept {
    return type;
}

UniformValueType Uniform::getValueType() const noexcept {
    return value_type;
}

GLint Uniform::getLocation() const noexcept {
    return location;
}

const std::string& Uniform::getName() const noexcept {
    return name;
}

bool Uniform::isChanged() const noexcept {
    return changed;
}

void Uniform::resetChanged() noexcept {
    changed = false;
}

void Uniform::setLocation(GLint loc) noexcept {
    location = loc;
}

Uniform::Uniform(const Uniform& rhs) noexcept
    : name {rhs.name}
    , location {rhs.location}
    , type {rhs.type}
    , value_type {rhs.value_type}
    , changed {true} {
}

bool Limitless::operator==(const Uniform& lhs, const Uniform& rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool Limitless::operator!=(const Uniform& lhs, const Uniform& rhs) noexcept {
    return !(lhs == rhs);
}

bool Limitless::operator<(const Uniform& lhs, const Uniform& rhs) noexcept {
    if (lhs.getType() != rhs.getType()) {
        return lhs.getType() < rhs.getType();
    }

    if (lhs.getValueType() != rhs.getValueType()) {
        return lhs.getValueType() < rhs.getValueType();
    }

    switch (lhs.getType()) {
        case UniformType::Sampler:
            return static_cast<const UniformSampler&>(lhs).getSampler() < static_cast<const UniformSampler&>(rhs).getSampler(); //NOLINT
        case UniformType::Value:
        case UniformType::Time:
            switch (lhs.getValueType()) {
                case UniformValueType::Float:
                    return static_cast<const UniformValue<float>&>(lhs).getValue() < static_cast<const UniformValue<float>&>(rhs).getValue(); //NOLINT
                case UniformValueType::Int:
                    return static_cast<const UniformValue<int>&>(lhs).getValue() < static_cast<const UniformValue<int>&>(rhs).getValue(); //NOLINT
                case UniformValueType::Uint:
                    return static_cast<const UniformValue<unsigned int>&>(lhs).getValue() < static_cast<const UniformValue<unsigned int>&>(rhs).getValue(); //NOLINT
                case UniformValueType::Vec2: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::vec2>&>(lhs).getValue(); //NOLINT
                    const auto& rhs_v = static_cast<const UniformValue<glm::vec2>&>(rhs).getValue(); //NOLINT
                    return std::tie(lhs_v.x, lhs_v.y) < std::tie(rhs_v.x, rhs_v.y);
                }
                case UniformValueType::Vec3: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::vec3>&>(lhs).getValue(); //NOLINT
                    const auto& rhs_v = static_cast<const UniformValue<glm::vec3>&>(rhs).getValue(); //NOLINT
                    return std::tie(lhs_v.x, lhs_v.y, lhs_v.z) < std::tie(rhs_v.x, rhs_v.y, rhs_v.z);
                }
                case UniformValueType::Vec4: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::vec4>&>(lhs).getValue(); //NOLINT
                    const auto& rhs_v = static_cast<const UniformValue<glm::vec4>&>(rhs).getValue(); //NOLINT
                    return std::tie(lhs_v.x, lhs_v.y, lhs_v.z, lhs_v.w) < std::tie(rhs_v.x, rhs_v.y, rhs_v.z, rhs_v.w);
                }
                case UniformValueType::Mat3: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::mat3>&>(lhs).getValue(); //NOLINT
                    const auto& rhs_v = static_cast<const UniformValue<glm::mat3>&>(rhs).getValue(); //NOLINT
                    return std::tie(lhs_v[0][0], lhs_v[0][1], lhs_v[0][2],
                                    lhs_v[1][0], lhs_v[1][1], lhs_v[1][2],
                                    lhs_v[2][1], lhs_v[2][1], lhs_v[2][2])
                                    <
                           std::tie(rhs_v[0][0], rhs_v[0][1], rhs_v[0][2],
                                    rhs_v[1][0], rhs_v[1][1], rhs_v[1][2],
                                    rhs_v[2][1], rhs_v[2][1], rhs_v[2][2]);
                }
                case UniformValueType::Mat4: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::mat3>&>(lhs).getValue(); //NOLINT
                    const auto& rhs_v = static_cast<const UniformValue<glm::mat3>&>(rhs).getValue(); //NOLINT
                    return std::tie(lhs_v[0][0], lhs_v[0][1], lhs_v[0][2], lhs_v[0][3],
                                    lhs_v[1][0], lhs_v[1][1], lhs_v[1][2], lhs_v[1][3],
                                    lhs_v[2][0], lhs_v[2][1], lhs_v[2][2], lhs_v[2][3],
                                    lhs_v[3][0], lhs_v[3][1], lhs_v[3][2], lhs_v[3][3])
                            <
                            std::tie(rhs_v[0][0], rhs_v[0][1], rhs_v[0][2], rhs_v[0][3],
                                     rhs_v[1][0], rhs_v[1][1], rhs_v[1][2], rhs_v[1][3],
                                     rhs_v[2][0], rhs_v[2][1], rhs_v[2][2], rhs_v[2][3],
                                     rhs_v[3][0], rhs_v[3][1], rhs_v[3][2], rhs_v[3][3]);
                }
            }
    }

    // unreal case because all switch cases are handled
    // just for warning
    return false;
}

std::string Limitless::getUniformDeclaration(const Uniform& uniform) noexcept {
    std::string declaration = "uniform ";

    switch (uniform.getType()) {
        case UniformType::Time:
        case UniformType::ValueArray:
        case UniformType::Value:
            switch (uniform.getValueType()) {
                case UniformValueType::Float:
                    declaration.append("float ");
                    break;
                case UniformValueType::Int:
                    declaration.append("int ");
                    break;
                case UniformValueType::Uint:
                    declaration.append("uint ");
                    break;
                case UniformValueType::Vec2:
                    declaration.append("vec2 ");
                    break;
                case UniformValueType::Vec3:
                    declaration.append("vec3 ");
                    break;
                case UniformValueType::Vec4:
                    declaration.append("vec4 ");
                    break;
                case UniformValueType::Mat4:
                    declaration.append("mat4 ");
                    break;
                case UniformValueType::Mat3:
                    declaration.append("mat3 ");
                    break;
                case DataType::IVec4:
                    declaration.append("ivec4 ");
                    break;
            }
            break;
        case UniformType::Sampler:
            const auto& sampler = static_cast<const UniformSampler&>(uniform).getSampler(); //NOLINT
            switch (sampler->getType()) {
                case Texture::Type::Tex2D:
                    declaration.append("sampler2D ");
                    break;
                case Texture::Type::Tex3D:
                    declaration.append("sampler3D ");
                    break;
                case Texture::Type::CubeMap:
                    declaration.append("samplerCube ");
                    break;
                case Texture::Type::Tex2DArray:
                    declaration.append("sampler2DArray ");
                    break;
                case Texture::Type::TexCubeMapArray:
                    declaration.append("samplerCubeArray ");
                    break;
            }
            break;
    }

    if (uniform.getType() != UniformType::ValueArray) {
        declaration.append(uniform.getName() + ";\n");
    } else {
        switch (uniform.getValueType()) {
            case UniformValueType::Float: {
                    auto count = static_cast<const UniformValueArray<float>&>(uniform).getCount();
                    declaration.append(uniform.getName() + "[" + std::to_string(count) + "];\n");
                }
                break;
            case UniformValueType::Int: {
                    auto count = static_cast<const UniformValueArray<int32_t>&>(uniform).getCount();
                    declaration.append(uniform.getName() + "[" + std::to_string(count) + "];\n");
                }
                break;
            case UniformValueType::Uint: {
                    auto count = static_cast<const UniformValueArray<uint32_t>&>(uniform).getCount();
                    declaration.append(uniform.getName() + "[" + std::to_string(count) + "];\n");
                }
                break;
            case UniformValueType::Vec2: {
                    auto count = static_cast<const UniformValueArray<glm::vec2>&>(uniform).getCount();
                    declaration.append(uniform.getName() + "[" + std::to_string(count) + "];\n");
                }
                break;
            case UniformValueType::Vec3: {
                    auto count = static_cast<const UniformValueArray<glm::vec3>&>(uniform).getCount();
                    declaration.append(uniform.getName() + "[" + std::to_string(count) + "];\n");
                }
                break;
            case UniformValueType::Vec4: {
                    auto count = static_cast<const UniformValueArray<glm::vec4>&>(uniform).getCount();
                    declaration.append(uniform.getName() + "[" + std::to_string(count) + "];\n");
                }
                break;
            case UniformValueType::Mat4: {
                    auto count = static_cast<const UniformValueArray<glm::mat4>&>(uniform).getCount();
                    declaration.append(uniform.getName() + "[" + std::to_string(count) + "];\n");
                }
                break;
            case UniformValueType::Mat3: {
                    auto count = static_cast<const UniformValueArray<glm::mat3>&>(uniform).getCount();
                    declaration.append(uniform.getName() + "[" + std::to_string(count) + "];\n");
                }
                break;
            case DataType::IVec4: {
                    auto count = static_cast<const UniformValueArray<glm::ivec4>&>(uniform).getCount();
                    declaration.append(uniform.getName() + "[" + std::to_string(count) + "];\n");
                }
                break;
        }
    }

    return declaration;
}

size_t Limitless::getUniformSize(const Uniform& uniform) {
    switch (uniform.getType()) {
        case UniformType::Value:
            switch (uniform.getValueType()) {
                case UniformValueType::Float:
                    return sizeof(float);
                case UniformValueType::Int:
                    return sizeof(int32_t);
                case UniformValueType::Uint:
                    return sizeof(uint32_t);
                case UniformValueType::Vec2:
                    return sizeof(glm::vec2);
                case UniformValueType::Vec3:
                    return sizeof(glm::vec3);
                case UniformValueType::Vec4:
                    return sizeof(glm::vec4);
                case UniformValueType::Mat3:
                    return sizeof(glm::mat3);
                case UniformValueType::Mat4:
                    return sizeof(glm::mat4);
                case DataType::IVec4:
                    return sizeof(glm::ivec4);
            }
            break;
        case UniformType::Sampler: {
            size_t size = 0;
            auto getter = TextureUniformSizeGetter {size};
            static_cast<const UniformSampler&>(uniform).getSampler()->accept(getter); //NOLINT
            return size;
        }
        case UniformType::Time:
            return sizeof(float);
        case UniformType::ValueArray:
            switch (uniform.getValueType()) {
                case UniformValueType::Float:
                    return sizeof(float) * static_cast<const UniformValueArray<float>&>(uniform).getValues().size(); //NOLINT
                case UniformValueType::Int:
                    return sizeof(int32_t) * static_cast<const UniformValueArray<float>&>(uniform).getValues().size(); //NOLINT
                case UniformValueType::Uint:
                    return sizeof(uint32_t) * static_cast<const UniformValueArray<float>&>(uniform).getValues().size(); //NOLINT
                case UniformValueType::Vec2:
                    return sizeof(glm::vec2) * static_cast<const UniformValueArray<float>&>(uniform).getValues().size(); //NOLINT
                case UniformValueType::Vec3:
                    return sizeof(glm::vec3) * static_cast<const UniformValueArray<float>&>(uniform).getValues().size(); //NOLINT
                case UniformValueType::Vec4:
                    return sizeof(glm::vec4) * static_cast<const UniformValueArray<float>&>(uniform).getValues().size(); //NOLINT
                case UniformValueType::Mat3:
                    return sizeof(glm::mat3) * static_cast<const UniformValueArray<float>&>(uniform).getValues().size(); //NOLINT
                case UniformValueType::Mat4:
                    return sizeof(glm::mat4) * static_cast<const UniformValueArray<float>&>(uniform).getValues().size(); //NOLINT
                case DataType::IVec4:
                    return sizeof(glm::ivec4) * static_cast<const UniformValueArray<float>&>(uniform).getValues().size(); //NOLINT
            }
            break;
    }

    return 0;
}

size_t Limitless::getUniformAlignment(const Uniform& uniform) {
    switch (uniform.getType()) {
        case UniformType::Value:
            switch (uniform.getValueType()) {
                case UniformValueType::Float:
                    return sizeof(float);
                case UniformValueType::Int:
                    return sizeof(int32_t);
                case UniformValueType::Uint:
                    return sizeof(uint32_t);
                case UniformValueType::Vec2:
                    return sizeof(glm::vec2);
                // All these have !vec4! alignment
                case UniformValueType::Vec3:
                case UniformValueType::Vec4:
                case UniformValueType::IVec4:
                case UniformValueType::Mat3:
                case UniformValueType::Mat4:
                    return sizeof(glm::vec4);
            }
            break;
        case UniformType::Sampler: {
            size_t alignment = 0;
            auto getter = TextureUniformAlignmentGetter {alignment};
            static_cast<const UniformSampler&>(uniform).getSampler()->accept(getter); //NOLINT
            return alignment;
        }
        case UniformType::Time:
            return sizeof(float);
        case UniformType::ValueArray:
            return sizeof(glm::vec4);
    }

    return 0;
}
