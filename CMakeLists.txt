cmake_minimum_required(VERSION 3.10)
project(limitless_engine)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -ggdb -Wpedantic -Wextra -Wall -Wnon-virtual-dtor -Wunused -Wno-attributes)
    add_compile_definitions(NDEBUG GL_DEBUG DEBUG)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Ofast -march=native)
endif()

if (OPENGL_DEBUG)
    add_compile_definitions(GL_DEBUG)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include_directories(include)

find_package(freetype REQUIRED)
message(STATUS "Found FREETYPE in ${FREETYPE_INCLUDE_DIRS}")
include_directories(${FREETYPE_INCLUDE_DIRS})
link_directories(${FREETYPE_LIBRARIES})

if (WIN32)
    find_package(stb_image REQUIRED)
    message(STATUS "Found stb_image in ${STB_IMAGE_INCLUDE_DIRS}")
    include_directories(${STB_IMAGE_INCLUDE_DIRS})

    find_package(glm REQUIRED)
    message(STATUS "GLM included at ${GLM_INCLUDE_DIRS}")
    include_directories(${GLM_INCLUDE_DIRS})
    link_directories(${GLM_LIBRARIES})

    find_package(glfw3 REQUIRED)
    message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIRS}")
    include_directories(${GLFW3_INCLUDE_DIRS})
    link_directories(${GLFW3_LIBRARIES})

    find_package(glew REQUIRED)
    message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIRS}")
    include_directories(${GLEW_INCLUDE_DIRS})
    link_directories(${GLEW_LIBRARIES})

    find_package(assimp REQUIRED)
    message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIRS}")
    include_directories(${ASSIMP_INCLUDE_DIRS})
    link_directories(${ASSIMP_LIBRARIES})

    find_package(stb_image REQUIRED)
    message(STATUS "Found stb_image in ${STB_IMAGE_INCLUDE_DIRS}")
    include_directories(${STB_IMAGE_INCLUDE_DIRS})

    link_libraries(opengl32 glew32 glfw3 assimp freetype)
endif()

if (UNIX)
    link_libraries(GLEW glfw GL assimp freetype pthread)
endif()

set(ENGINE_CORE
    src/limitless/core/context_debug.cpp
    src/limitless/core/context_initializer.cpp
    src/limitless/core/context_state.cpp
    src/limitless/core/context.cpp
    src/limitless/core/context_observer.cpp
    src/limitless/core/state_query.cpp

    src/limitless/core/mutable_texture.cpp
    src/limitless/core/immutable_texture.cpp
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
)

set(ENGINE_INSTANCES
    src/limitless/instances/abstract_instance.cpp
    src/limitless/instances/effect_attachable.cpp
    src/limitless/instances/light_attachable.cpp
    src/limitless/instances/skeletal_instance.cpp
    src/limitless/instances/mesh_instance.cpp
    src/limitless/instances/model_instance.cpp
    src/limitless/instances/effect_instance.cpp
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
    src/limitless/loaders/asset_loader.cpp
    src/limitless/loaders/threaded_model_loader.cpp
    src/limitless/loaders/texture_loader.cpp
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

add_library(limitless_engine_objects OBJECT ${ENGINE_SRC})
add_library(limitless_engine SHARED ${ENGINE_SRC})

add_executable(limitless_demo
        $<TARGET_OBJECTS:limitless_engine_objects>
        demo/demo.cpp
        )

add_executable(limitless_engine_tests
        $<TARGET_OBJECTS:limitless_engine_objects>
        "tests/catch_amalgamated.cpp"

        "tests/core/texture_tests.cpp")

add_compile_definitions(ENGINE_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
