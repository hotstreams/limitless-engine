#########################################
cmake_minimum_required(VERSION 3.10)

#########################################
project(limitless-engine)

#########################################

OPTION(BUILD_SAMPLES "Builds samples" ON)
OPTION(BUILD_TESTS "Builds tests" ON)

OPTION(OPENGL_DEBUG "Enables debug mode for OpenGL" ON)
OPTION(OPENGL_NO_EXTENSIONS "Disables all extensions" OFF)
OPTION(OPENGL_SHADER_OUTPUT "Outputs all source shaders for GLSLANG testing" OFF)

#########################################

set(ENGINE_CORE
    src/limitless/core/context_debug.cpp
    src/limitless/core/context_initializer.cpp
    src/limitless/core/context_state.cpp
    src/limitless/core/context.cpp
    src/limitless/core/window_hints.cpp
    src/limitless/core/context_observer.cpp
    src/limitless/core/state_query.cpp
    src/limitless/core/profiler.cpp
    src/limitless/core/time_query.cpp

    src/limitless/core/texture/texture.cpp
    src/limitless/core/texture/state_texture.cpp
    src/limitless/core/texture/named_texture.cpp
    src/limitless/core/texture/bindless_texture.cpp
    src/limitless/core/texture/texture_builder.cpp
    src/limitless/core/texture/texture_uniform_alignment_getter.cpp
    src/limitless/core/texture/texture_uniform_size_getter.cpp
    src/limitless/core/texture/texture_uniform_setter.cpp
    src/limitless/core/texture/texture_extension_capturer.cpp
    src/limitless/core/texture/texture_resident_maker.cpp

    src/limitless/core/buffer/state_buffer.cpp
    src/limitless/core/buffer/named_buffer.cpp
    src/limitless/core/buffer/triple_buffer.cpp
    src/limitless/core/buffer/indexed_buffer.cpp
    src/limitless/core/buffer/buffer_builder.cpp

    src/limitless/core/uniform/uniform.cpp
    src/limitless/core/uniform/uniform_value.cpp
    src/limitless/core/uniform/uniform_sampler.cpp
    src/limitless/core/uniform/uniform_time.cpp
    src/limitless/core/uniform/uniform_setter.cpp

    src/limitless/core/shader/shader.cpp
    src/limitless/core/shader/shader_program.cpp
    src/limitless/core/shader/shader_program_texture_setter.cpp
    src/limitless/core/shader/shader_compiler.cpp
    src/limitless/core/shader/shader_define_replacer.cpp

    src/limitless/core/vertex_array.cpp
    src/limitless/core/framebuffer.cpp

    src/limitless/core/texture/texture_binder.cpp
    src/limitless/core/context_thread_pool.cpp
    src/limitless/core/sync.cpp

    src/limitless/core/buffer/buffer_binding_point.cpp
    src/limitless/core/shader/shader_program_introspection.cpp
)

set(ENGINE_INSTANCES
    src/limitless/instances/instance.cpp
    src/limitless/instances/skeletal_instance.cpp
    src/limitless/instances/mesh_instance.cpp
    src/limitless/instances/model_instance.cpp
    src/limitless/instances/effect_instance.cpp
    src/limitless/instances/instance_attachment.cpp
    src/limitless/instances/socket_attachment.cpp
    src/limitless/instances/instance_builder.cpp
    src/limitless/instances/decal_instance.cpp
    src/limitless/instances/instanced_instance.cpp
    src/limitless/instances/terrain_instance.cpp
)

set(ENGINE_LIGHTING
    src/limitless/lighting/lighting.cpp
    src/limitless/lighting/light_container.cpp
    src/limitless/lighting/cascade_shadows.cpp
    src/limitless/lighting/light.cpp
)

set(ENGINE_LOADERS
    src/limitless/loaders/material_loader.cpp
    src/limitless/loaders/effect_loader.cpp
#    src/limitless/loaders/asset_manager.cpp
#    src/limitless/loaders/threaded_model_loader.cpp
    src/limitless/loaders/texture_loader.cpp
    src/limitless/loaders/dds_loader.cpp
    src/limitless/loaders/cgltf.c
    src/limitless/loaders/gltf_model_loader.cpp
)

set(ENGINE_MODELS
    src/limitless/models/elementary_model.cpp
    src/limitless/models/text_model.cpp
    src/limitless/models/skeletal_model.cpp
    src/limitless/models/abstract_model.cpp
    src/limitless/models/cube.cpp
    src/limitless/models/line.cpp
    src/limitless/models/plane.cpp
    src/limitless/models/quad.cpp
    src/limitless/models/sphere.cpp
    src/limitless/models/model.cpp
    src/limitless/models/cylinder.cpp
)

set(ENGINE_SERIALIZATION
    src/limitless/serialization/effect_serializer.cpp
    src/limitless/serialization/emitter_serializer.cpp
    src/limitless/serialization/distribution_serializer.cpp
    src/limitless/serialization/material_serializer.cpp
    src/limitless/serialization/uniform_serializer.cpp
)

set(ENGINE_UTIL
    src/limitless/util/thread_pool.cpp
    src/limitless/util/sorter.cpp
    src/limitless/util/renderer_helper.cpp
    src/limitless/util/color_picker.cpp
)

set(ENGINE_MS
    src/limitless/ms/blending.cpp
    src/limitless/ms/material.cpp
    src/limitless/ms/unique_material.cpp
    src/limitless/ms/material_builder.cpp
    src/limitless/ms/material_compiler.cpp
    src/limitless/ms/material_buffer.cpp
    src/limitless/ms/material_shader_define_replacer.cpp
)

set(ENGINE_TEXT
    src/limitless/text/text_instance.cpp
    src/limitless/text/font_atlas.cpp
)

set(ENGINE_FX
    src/limitless/fx/emitters/abstract_emitter.cpp
    src/limitless/fx/emitters/emitter.cpp
    src/limitless/fx/emitters/sprite_emitter.cpp
    src/limitless/fx/emitters/mesh_emitter.cpp
    src/limitless/fx/emitters/beam_emitter.cpp
    src/limitless/fx/emitters/unique_emitter.cpp

    src/limitless/fx/effect_renderer.cpp
    src/limitless/fx/effect_builder.cpp
    src/limitless/fx/effect_compiler.cpp
    src/limitless/fx/particle.cpp
    src/limitless/fx/effect_shader_define_replacer.cpp
)

set(ENGINE_PIPELINE
    src/limitless/pipeline/pipeline.cpp
    src/limitless/pipeline/pipeline_pass.cpp
    src/limitless/pipeline/forward/color_pass.cpp
    src/limitless/pipeline/forward/particle_pass.cpp
    src/limitless/pipeline/forward/framebuffer_pass.cpp
    src/limitless/pipeline/common/shadow_pass.cpp
    src/limitless/pipeline/common/sceneupdate_pass.cpp
    src/limitless/pipeline/common/skybox_pass.cpp
    src/limitless/pipeline/forward/postprocessing_pass.cpp
    src/limitless/pipeline/forward/forward.cpp
    src/limitless/pipeline/forward/postprocessing.cpp
    src/limitless/renderer/renderer.cpp
    src/limitless/renderer/render_settings_shader_definer.cpp
    src/limitless/renderer/renderer_settings.cpp
    src/limitless/pipeline/common/scene_data.cpp
    src/limitless/pipeline/deferred/effectupdate_pass.cpp
    src/limitless/pipeline/deferred/deferred_framebuffer_pass.cpp
    src/limitless/pipeline/deferred/gbuffer_pass.cpp
    src/limitless/pipeline/deferred/deferred_lighting_pass.cpp
    src/limitless/pipeline/deferred/deferred.cpp
    src/limitless/pipeline/deferred/depth_pass.cpp
    src/limitless/pipeline/deferred/translucent_pass.cpp
    src/limitless/pipeline/common/bloom_pass.cpp
    src/limitless/pipeline/deferred/composite_pass.cpp
    src/limitless/pipeline/common/ssao_pass.cpp
    src/limitless/pipeline/common/fxaa_pass.cpp
    src/limitless/pipeline/common/quad_pass.cpp
    src/limitless/pipeline/common/render_debug_pass.cpp
    src/limitless/pipeline/common/dof_pass.cpp
    src/limitless/pipeline/common/outline_pass.cpp
    src/limitless/pipeline/common/ssr_pass.cpp
    src/limitless/pipeline/deferred/decal_pass.cpp
    src/limitless/postprocessing/blur.cpp
    src/limitless/postprocessing/bloom.cpp
    src/limitless/postprocessing/ssao.cpp
    src/limitless/postprocessing/ssr.cpp
)

set(ENGINE_SRC
    ${ENGINE_CORE}
    ${ENGINE_MS}
    ${ENGINE_FX}
    ${ENGINE_INSTANCES}
    ${ENGINE_LIGHTING}
    ${ENGINE_LOADERS}
    ${ENGINE_SERIALIZATION}
    ${ENGINE_MODELS}
    ${ENGINE_UTIL}
    ${ENGINE_TEXT}
    ${ENGINE_PIPELINE}

    src/limitless/camera.cpp
    src/limitless/shader_storage.cpp
    src/limitless/assets.cpp
    src/limitless/scene.cpp
    src/limitless/skybox/skybox.cpp
    src/limitless/log.cpp
)

#########################################
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/thirdparty/freetype")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/thirdparty/glew/build/cmake")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/plog")

    target_compile_options(glfw PUBLIC -O3)
    target_compile_options(freetype PUBLIC -O3)
    target_compile_options(glew PUBLIC -O3)

##########################################
add_library(limitless-engine STATIC ${ENGINE_SRC})

target_include_directories(limitless-engine PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glfw/include")
target_include_directories(limitless-engine PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/stbimage")
target_include_directories(limitless-engine PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glm")
target_include_directories(limitless-engine PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/freetype/include")
target_include_directories(limitless-engine PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glew/include")
target_include_directories(limitless-engine PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/plog/include")

target_link_libraries(limitless-engine PUBLIC glfw ${GLFW_LIBRARIES})
target_link_libraries(limitless-engine PUBLIC glew ${GLEW_LIBRARIES})
target_link_libraries(limitless-engine PUBLIC freetype)

if (UNIX)
    target_link_libraries(limitless-engine PUBLIC GL)
endif()

target_include_directories(limitless-engine PUBLIC include)

target_compile_features(limitless-engine PUBLIC cxx_std_17)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(limitless-engine PRIVATE -O0 -ggdb -Wpedantic -Wextra -Wall -Wunused -Wno-attributes)
    target_compile_definitions(limitless-engine PUBLIC LIMITLESS_OPENGL_DEBUG)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
#    target_compile_options(limitless-engine PRIVATE -Wpedantic -Wextra -Wall -Wunused -Wno-attributes -Werror -O3)
    target_compile_options(limitless-engine PRIVATE -Wpedantic -Wextra -Wall -Wunused -Wno-attributes -O3)
endif()

if (OPENGL_DEBUG)
    target_compile_definitions(limitless-engine PUBLIC LIMITLESS_OPENGL_DEBUG)
endif()

if (OPENGL_NO_EXTENSIONS)
    target_compile_definitions(limitless-engine PUBLIC LIMITLESS_OPENGL_NO_EXTENSIONS)
endif()

if (OPENGL_SHADER_OUTPUT)
    target_compile_definitions(limitless-engine PUBLIC LIMITLESS_OPENGL_SHADER_OUTPUT)
endif()

if (NOT LIMITLESS_ASSETS_DIR)
    set(LIMITLESS_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/")
endif()

if (NOT LIMITLESS_SHADERS_DIR)
    set(LIMITLESS_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders/")
endif()

target_compile_definitions(limitless-engine PUBLIC ENGINE_ASSETS_DIR="${LIMITLESS_ASSETS_DIR}")
target_compile_definitions(limitless-engine PUBLIC ENGINE_SHADERS_DIR="${LIMITLESS_SHADERS_DIR}")

add_subdirectory(samples)
add_subdirectory(tests)