#include <limitless/core/uniform/uniform.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/uniform/uniform_value.hpp>

#include <limitless/core/shader_program.hpp>
#include <limitless/core/bindless_texture.hpp>
#include <limitless/core/texture.hpp>
#include <limitless/core/context_initializer.hpp>

using namespace Limitless;

Uniform::Uniform(std::string name, UniformType type, UniformValueType value_type) noexcept
    : name{std::move(name)}
    , type{type}
    , value_type{value_type}
    , changed{true} {}

bool Limitless::operator<(const Uniform& lhs, const Uniform& rhs) noexcept {
    //TODO: move to util/glm
    if (lhs.type != rhs.type) {
        return lhs.type < rhs.type;
    }

    if (lhs.value_type != rhs.value_type) {
        return lhs.value_type < rhs.value_type;
    }

    switch (lhs.type) {
        case UniformType::Sampler:
            return static_cast<const UniformSampler&>(lhs).getSampler() < static_cast<const UniformSampler&>(rhs).getSampler();
        case UniformType::Value:
        case UniformType::Time:
            switch (lhs.value_type) {
                case UniformValueType::Float:
                    return static_cast<const UniformValue<float>&>(lhs).getValue() < static_cast<const UniformValue<float>&>(rhs).getValue();
                case UniformValueType::Int:
                    return static_cast<const UniformValue<int>&>(lhs).getValue() < static_cast<const UniformValue<int>&>(rhs).getValue();
                case UniformValueType::Uint:
                    return static_cast<const UniformValue<unsigned int>&>(lhs).getValue() < static_cast<const UniformValue<unsigned int>&>(rhs).getValue();
                case UniformValueType::Vec2: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::vec2>&>(lhs).getValue();
                    const auto& rhs_v = static_cast<const UniformValue<glm::vec2>&>(rhs).getValue();
                    return std::tie(lhs_v.x, lhs_v.y) < std::tie(rhs_v.x, rhs_v.y);
                }
                case UniformValueType::Vec3: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::vec3>&>(lhs).getValue();
                    const auto& rhs_v = static_cast<const UniformValue<glm::vec3>&>(rhs).getValue();
                    return std::tie(lhs_v.x, lhs_v.y, lhs_v.z) < std::tie(rhs_v.x, rhs_v.y, rhs_v.z);
                }
                case UniformValueType::Vec4: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::vec4>&>(lhs).getValue();
                    const auto& rhs_v = static_cast<const UniformValue<glm::vec4>&>(rhs).getValue();
                    return std::tie(lhs_v.x, lhs_v.y, lhs_v.z, lhs_v.w) < std::tie(rhs_v.x, rhs_v.y, rhs_v.z, rhs_v.w);
                }
                case UniformValueType::Mat3: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::mat3>&>(lhs).getValue();
                    const auto& rhs_v = static_cast<const UniformValue<glm::mat3>&>(rhs).getValue();
                    return std::tie(lhs_v[0][0], lhs_v[0][1], lhs_v[0][2],
                                    lhs_v[1][0], lhs_v[1][1], lhs_v[1][2],
                                    lhs_v[2][1], lhs_v[2][1], lhs_v[2][2])
                                    <
                                    std::tie(rhs_v[0][0], rhs_v[0][1], rhs_v[0][2],
                                             rhs_v[1][0], rhs_v[1][1], rhs_v[1][2],
                                             rhs_v[2][1], rhs_v[2][1], rhs_v[2][2]);
                }
                case UniformValueType::Mat4: {
                    const auto& lhs_v = static_cast<const UniformValue<glm::mat3>&>(lhs).getValue();
                    const auto& rhs_v = static_cast<const UniformValue<glm::mat3>&>(rhs).getValue();
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

bool Limitless::operator==(const Uniform& lhs, const Uniform& rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool Limitless::operator!=(const Uniform& lhs, const Uniform& rhs) noexcept {
    return !(lhs == rhs);
}

std::string Limitless::getUniformDeclaration(const Uniform& uniform) noexcept {
    std::string declaration = "uniform ";

    switch (uniform.getType()) {
        case UniformType::Time:
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
            }
            break;
        case UniformType::Sampler:
            const auto& sampler = static_cast<const UniformSampler&>(uniform).getSampler();
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
//                case Texture::Type::Tex2DMS:
//                    declaration.append("gsampler2DMS ");
//                    break;
            }
            break;
    }

    declaration.append(uniform.getName() + ";\n");

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
            }
            break;
        case UniformType::Sampler:
            return ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture") ? sizeof(uint64_t) : 0;
        case UniformType::Time:
            return sizeof(float);
    }

    // unreal case because all switch cases are handled
    // just for warning
    return false;
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
                case UniformValueType::Vec3:
                    return sizeof(glm::vec4); // red zone
                case UniformValueType::Vec4:
                    return sizeof(glm::vec4);
                case UniformValueType::Mat3:
                    return sizeof(glm::vec4); // red zone
                case UniformValueType::Mat4:
                    return sizeof(glm::vec4); // red zone
            }
            break;
        case UniformType::Sampler:
            return ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture") ? sizeof(uint64_t) : 0;
        case UniformType::Time:
            return sizeof(float);
    }

    // unreal case because all switch cases are handled
    // just for warning
    return false;
}
