#pragma once

namespace Limitless {
    constexpr auto bindless_texture = "GL_ARB_bindless_texture";
    constexpr auto bindless_texture_define = "#define ENGINE_EXT_BINDLESS_TEXTURE\n";
    constexpr auto extension_bindless_texture = "#extension GL_ARB_bindless_texture : require\n";
    constexpr auto bindless_samplers = "layout(bindless_sampler) uniform;\n";

    constexpr auto shader_storage_buffer_object = "GL_ARB_shader_storage_buffer_object";
    constexpr auto extension_shader_storage_buffer_object = "#extension GL_ARB_shader_storage_buffer_object : require\n";

    constexpr auto shading_language_420pack = "GL_ARB_shading_language_420pack";
    constexpr auto extension_shading_language_420pack = "#extension GL_ARB_shading_language_420pack : require\n";

    constexpr auto explicit_uniform_location = "GL_ARB_explicit_uniform_location";
    constexpr auto extension_explicit_uniform_location = "#extension GL_ARB_explicit_uniform_location : require\n";
}