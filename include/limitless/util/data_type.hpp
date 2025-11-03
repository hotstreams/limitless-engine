#pragma once

#include <limitless/core/context_debug.hpp>

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <string>

namespace Limitless {
    enum class DataType {
        Float,
        Int,
        Uint,
        Vec2,
        Vec3,
        Vec4,
        Mat3,
        Mat4,
        IVec4
    };

    // returns <count, type>
    inline std::pair<uint8_t, GLenum> getOpenGLDataType(DataType type) {
        switch (type) {
            case DataType::Float: return {1, GL_FLOAT};
            case DataType::Int: return {1, GL_INT};
            case DataType::Uint: return {1, GL_UNSIGNED_INT};
            case DataType::Vec2: return {2, GL_FLOAT};
            case DataType::Vec3: return {3, GL_FLOAT};
            case DataType::Vec4: return {4, GL_FLOAT};
            case DataType::IVec4: return {4, GL_INT};
            case DataType::Mat3: return {9, GL_FLOAT};
            case DataType::Mat4: return {16, GL_FLOAT};
        }
    }

    inline DataType getDataType(GLenum type, GLint size) {
        switch (type) {
            case GL_FLOAT: {
                switch (size) {
                    case 1: return DataType::Float;
                    case 2: return DataType::Vec2;
                    case 3: return DataType::Vec3;
                    case 4: return DataType::Vec4;
                    case 9: return DataType::Mat3;
                    case 16: return DataType::Mat4;
                    default: throw std::runtime_error("Unsupported data type");
                }
            }
            case GL_INT: {
                switch (size) {
                    case 1: return DataType::Int;
                    case 4: return DataType::IVec4;
                    default: throw std::runtime_error("Unsupported data type");
                }
            }
            case GL_UNSIGNED_INT: {
                switch (size) {
                    case 1: return DataType::Uint;
                    default: throw std::runtime_error("Unsupported data type");
                }
            }
            default:
                throw std::runtime_error("Unsupported data type");
        }
    }

    inline std::string getDataTypeString(DataType type) {
        switch (type) {
            case DataType::Float: return "float";
            case DataType::Int: return "int";
            case DataType::Uint: return "uint";
            case DataType::Vec2: return "vec2";
            case DataType::Vec3: return "vec3";
            case DataType::Vec4: return "vec4";
            case DataType::IVec4: return "ivec4";
            case DataType::Mat3: return "mat3";
            case DataType::Mat4: return "mat4";
        }
    }
}