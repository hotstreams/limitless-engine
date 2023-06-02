cmake_minimum_required(VERSION 3.10)
project(limitless_engine)

set(CMAKE_CXX_STANDARD 17)

# CONFIGURABLE OPTIONS

OPTION(DEBUG_OPENGL "Enables debug mode for OpenGL" OFF)
OPTION(OPENGL_TOSTER_MODE "Disables all extensions" OFF)
OPTION(BUILD_TESTS "Builds tests" OFF)
OPTION(WEBGL_BUILD "Web build" OFF)

#########################################

# SETTINGS COMPILE FLAGS AND DEFINITIONS

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIMITLESS_COMPILE_OPTIONS -O0 -ggdb -Wpedantic -Wextra -Wall -Wunused -Wno-attributes)
    add_compile_definitions(GL_DEBUG LIMITLESS_DEBUG)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(LIMITLESS_COMPILE_OPTIONS -Wpedantic -Wextra -Wall -Wunused -Wno-attributes -O3)
    add_compile_definitions(NDEBUG)
endif()

if (DEBUG_OPENGL)
    add_compile_definitions(GL_DEBUG)
endif()

if (OPENGL_TOSTER_MODE)
    add_compile_definitions(OPENGL_TOSTER)
endif()

if (NOT LIMITLESS_ASSETS_DIR)
    set(LIMITLESS_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/")
endif()

if (NOT LIMITLESS_SHADERS_DIR)
    set(LIMITLESS_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders/")
endif()

add_compile_definitions(ENGINE_ASSETS_DIR="${LIMITLESS_ASSETS_DIR}")
add_compile_definitions(ENGINE_SHADERS_DIR="${LIMITLESS_SHADERS_DIR}")

#########################################

include_directories(include)

set(ENGINE_CORE
    src/limitless/core/context_debug.cpp
    src/limitless/core/context_initializer.cpp
    src/limitless/core/context_state.cpp
    src/limitless/core/context.cpp
    src/limitless/core/context_observer.cpp
    src/limitless/core/state_query.cpp
    src/limitless/core/profiler.cpp
    src/limitless/core/time_query.cpp

    src/limitless/core/texture.cpp
    src/limitless/core/state_texture.cpp
    src/limitless/core/named_texture.cpp
    src/limitless/core/bindless_texture.cpp
    src/limitless/core/texture_builder.cpp

    src/limitless/core/state_buffer.cpp
    src/limitless/core/named_buffer.cpp
    src/limitless/core/triple_buffer.cpp
    src/limitless/core/indexed_buffer.cpp
    src/limitless/core/buffer_builder.cpp

    src/limitless/core/uniform.cpp
    src/limitless/core/uniform_setter.cpp
    src/limitless/core/shader.cpp
    src/limitless/core/shader_program.cpp
    src/limitless/core/shader_compiler.cpp

    src/limitless/core/vertex_array.cpp
    src/limitless/core/framebuffer.cpp

    src/limitless/core/texture_binder.cpp
    src/limitless/core/context_thread_pool.cpp
    src/limitless/core/sync.cpp
)

set(ENGINE_INSTANCES
    src/limitless/instances/abstract_instance.cpp
    src/limitless/instances/skeletal_instance.cpp
    src/limitless/instances/mesh_instance.cpp
    src/limitless/instances/model_instance.cpp
    src/limitless/instances/effect_instance.cpp
    src/limitless/instances/instance_attachment.cpp
)

set(ENGINE_LIGHTING
    src/limitless/lighting/lighting.cpp
    src/limitless/lighting/light_container.cpp
    src/limitless/lighting/cascade_shadows.cpp
)

set(ENGINE_LOADERS
    src/limitless/loaders/material_loader.cpp
    src/limitless/loaders/effect_loader.cpp
    src/limitless/loaders/model_loader.cpp
    src/limitless/loaders/asset_manager.cpp
    src/limitless/loaders/threaded_model_loader.cpp
    src/limitless/loaders/texture_loader.cpp
    src/limitless/loaders/dds_loader.cpp
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
    src/limitless/ms/material_instance.cpp
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
)

set(ENGINE_PIPELINE
    src/limitless/pipeline/pipeline.cpp
    src/limitless/pipeline/render_pass.cpp
    src/limitless/pipeline/color_pass.cpp
    src/limitless/pipeline/particle_pass.cpp
    src/limitless/pipeline/framebuffer_pass.cpp
    src/limitless/pipeline/shadow_pass.cpp
    src/limitless/pipeline/sceneupdate_pass.cpp
    src/limitless/pipeline/skybox_pass.cpp
    src/limitless/pipeline/postprocessing_pass.cpp
    src/limitless/pipeline/forward.cpp
    src/limitless/pipeline/postprocessing.cpp
    src/limitless/pipeline/renderer.cpp
    src/limitless/pipeline/scene_data.cpp
    src/limitless/pipeline/effectupdate_pass.cpp
    src/limitless/pipeline/deferred_framebuffer_pass.cpp
    src/limitless/pipeline/gbuffer_pass.cpp
    src/limitless/pipeline/deferred_lighting_pass.cpp
    src/limitless/pipeline/deferred.cpp
    src/limitless/pipeline/depth_pass.cpp
    src/limitless/pipeline/translucent_pass.cpp
    src/limitless/pipeline/blur_pass.cpp
    src/limitless/pipeline/composite_pass.cpp
    src/limitless/pipeline/ssao_pass.cpp
    src/limitless/pipeline/fxaa_pass.cpp
    src/limitless/pipeline/quad_pass.cpp
    src/limitless/pipeline/render_debug_pass.cpp
    src/limitless/pipeline/dof_pass.cpp
    src/limitless/pipeline/outline_pass.cpp
    src/limitless/pipeline/blur.cpp
    src/limitless/pipeline/bloom.cpp
)

set(ENGINE_SRC
    ${ENGINE_CORE}
    ${ENGINE_MS}
    ${ENGINE_FX}
    ${ENGINE_INSTANCES}
    ${ENGINE_LIGHTING}
    ${ENGINE_LOADERS}
    ${ENGINE_MODELS}
    ${ENGINE_SERIALIZATION}
    ${ENGINE_UTIL}
    ${ENGINE_TEXT}
    ${ENGINE_PIPELINE}

    src/limitless/camera.cpp
    src/limitless/shader_storage.cpp
    src/limitless/assets.cpp
    src/limitless/scene.cpp
    src/limitless/skybox/skybox.cpp
)

##### CONFIGURE SUBMODULES
    # GLFW
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    if (NOT WEBGL_BUILD)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw")
    endif()
    #####

    # ASSIMP
    set(BUILD_SHARED_LIBS FALSE CACHE BOOL "x" FORCE)
    set(ASSIMP_NO_EXPORT TRUE CACHE BOOL "x" FORCE)
    set(ASSIMP_BUILD_TESTS FALSE CACHE BOOL "x" FORCE)
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT FALSE CACHE BOOL "x" FORCE)
    set(ASSIMP_INSTALL_PDB FALSE CACHE BOOL "x" FORCE)
    set(ASSIMP_BUILD_ZLIB TRUE CACHE BOOL "x" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS TRUE CACHE BOOL "x" FORCE)
    set(ASSIMP_BUILD_COLLADA_IMPORTER FALSE CACHE BOOL "x" FORCE)
    set(ASSIMP_BUILD_OBJ_IMPORTER TRUE CACHE BOOL "x" FORCE)
    set(ASSIMP_BUILD_FBX_IMPORTER TRUE CACHE BOOL "x" FORCE)
    set(ASSIMP_BUILD_MD5_IMPORTER TRUE CACHE BOOL "x" FORCE)
    set(ASSIMP_BUILD_BLEND_IMPORTER FALSE CACHE BOOL "x" FORCE)

    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/thirdparty/assimp")
    #####

    # FREETYPE
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/thirdparty/freetype")
    #####

    # GLEW
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/thirdparty/glew/build/cmake")
    #####

    # PLOG
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/plog")
    #####

    if (NOT WEBGL_BUILD)
        target_compile_options(glfw PUBLIC -O3)
    endif()
    target_compile_options(assimp PUBLIC -O3)
    target_compile_options(freetype PUBLIC -O3)
    target_compile_options(glew PUBLIC -O3)
##########################################

################# STATIC

add_library(limitless_engine_static STATIC ${ENGINE_SRC})

target_include_directories(limitless_engine_static PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glfw/include")
target_include_directories(limitless_engine_static PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/assimp/include" "${CMAKE_CURRENT_BINARY_DIR}/thirdparty/assimp/include")
target_include_directories(limitless_engine_static PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/stbimage")
target_include_directories(limitless_engine_static PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glm")
target_include_directories(limitless_engine_static PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/freetype/include")
target_include_directories(limitless_engine_static PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glew/include")
target_include_directories(limitless_engine_static PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glew/include")
target_include_directories(limitless_engine_static PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/plog/include")

target_link_libraries(limitless_engine_static PUBLIC glfw ${GLFW_LIBRARIES})
target_link_libraries(limitless_engine_static PUBLIC assimp ${ASSIMP_LIBRARIES})
target_link_libraries(limitless_engine_static PUBLIC freetype)
target_link_libraries(limitless_engine_static PUBLIC glew ${GLEW_LIBRARIES})

target_compile_options(limitless_engine_static PUBLIC "${LIMITLESS_COMPILE_OPTIONS}")

#######################

##############################################

#                DEMO

add_executable(limitless_demo
    $<TARGET_OBJECTS:limitless_engine_static>

    demo/demo.cpp
    demo/materials_demoscene.cpp
    demo/lighting_demoscene.cpp
    demo/effects_demoscene.cpp
    demo/models_demoscene.cpp
    demo/sponza_demoscene.cpp
    demo/assets.cpp
)

target_include_directories(limitless_demo PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glfw/include")
target_include_directories(limitless_demo PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/assimp/include" "${CMAKE_CURRENT_BINARY_DIR}/thirdparty/assimp/include")
target_include_directories(limitless_demo PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/stbimage")
target_include_directories(limitless_demo PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glm")
target_include_directories(limitless_demo PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/freetype/include")
target_include_directories(limitless_demo PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glew/include")
target_include_directories(limitless_demo PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glew/include")
target_include_directories(limitless_demo PUBLIC "${CMAKE_CURRENT_LIST_DIR}/thirdparty/plog/include")

target_link_libraries(limitless_demo glfw ${GLFW_LIBRARIES})
target_link_libraries(limitless_demo assimp ${ASSIMP_LIBRARIES})
target_link_libraries(limitless_demo freetype)
target_link_libraries(limitless_demo glew ${GLEW_LIBRARIES})
