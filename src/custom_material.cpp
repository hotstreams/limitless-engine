#include <custom_material.hpp>

using namespace GraphicsEngine;

CustomMaterial::CustomMaterial(decltype(properties)&& properties, decltype(uniform_offsets)&& offsets, Blending blending,
                               Shading shading, std::string name, uint64_t shader_index, decltype(uniforms)&& uniforms) noexcept
    : Material(std::move(properties), std::move(offsets), blending, shading, std::move(name), shader_index), uniforms{std::move(uniforms)}
{

}
