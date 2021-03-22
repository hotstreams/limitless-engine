cmake_minimum_required(VERSION 3.10)
project(limitless_engine)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -ggdb -Wpedantic -Wextra -Wall -Wnon-virtual-dtor -Wunused)
    add_compile_definitions(NDEBUG GL_DEBUG)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3)
endif()

if (OPENGL_DEBUG)
    add_compile_definitions(GL_DEBUG)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(stb_image REQUIRED)
message(STATUS "Found stb_image in ${STB_IMAGE_INCLUDE_DIRS}")
include_directories(${STB_IMAGE_INCLUDE_DIRS})

include_directories(include)

if (WIN32)
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

    find_package(freetype REQUIRED)
    message(STATUS "Found FREETYPE in ${FREETYPE_INCLUDE_DIRS}")
    include_directories(${FREETYPE_INCLUDE_DIRS})
    link_directories(${FREETYPE_LIBRARIES})

    link_libraries(opengl32 glew32 glfw3 assimp freetype)
endif()

if (UNIX)
    link_libraries(GLEW glfw GL assimp freetype pthread)
endif()

set(ENGINE_CORE
        src/limitless/core/bindless_texture.cpp
        src/limitless/core/buffer_builder.cpp
        src/limitless/core/context.cpp
        src/limitless/core/context_debug.cpp
        src/limitless/core/context_initializer.cpp
        src/limitless/core/context_observer.cpp
        src/limitless/core/context_state.cpp
        src/limitless/core/shader_compiler.cpp
        src/limitless/core/shader_program.cpp
        src/limitless/core/uniform.cpp
        src/limitless/serialization/uniform_serializer.cpp
        src/limitless/core/texture_binder.cpp
        src/limitless/core/vertex_array.cpp
        src/limitless/core/framebuffer.cpp
        src/limitless/core/state_buffer.cpp
        src/limitless/core/named_buffer.cpp
        src/limitless/core/triple_buffer.cpp
        src/limitless/core/indexed_buffer.cpp
        src/limitless/core/shader.cpp
        src/limitless/core/state_texture.cpp
        src/limitless/core/named_texture.cpp
        src/limitless/core/context_thread_pool.cpp
        src/limitless/core/mutable_texture.cpp
        src/limitless/core/immutable_texture.cpp
        src/limitless/core/texture_builder.cpp
   )

set(ENGINE_MATERIAL_SYSTEM
        src/limitless/material_system/properties.cpp
        src/limitless/material_system/material.cpp
        src/limitless/serialization/material_serializer.cpp
        src/limitless/material_system/material_builder.cpp
        src/limitless/material_system/custom_material.cpp
        src/limitless/material_system/custom_material_builder.cpp
        src/limitless/material_system/material_compiler.cpp
        src/limitless/material_system/material_instance.cpp
        src/limitless/loaders/material_loader.cpp
    )

set(ENGINE_PARTICLE_SYSTEM
        src/limitless/particle_system/emitter.cpp
        src/limitless/particle_system/emitter_module.cpp
        src/limitless/particle_system/sprite_emitter.cpp
        src/limitless/particle_system/mesh_emitter.cpp
        src/limitless/particle_system/effect_renderer.cpp
        src/limitless/particle_system/emitter_renderer.cpp
        src/limitless/particle_system/emitter_spawn.cpp
        src/limitless/particle_system/emitter_visitor.cpp
        src/limitless/particle_system/unique_emitter.cpp
        src/limitless/particle_system/effect_builder.cpp
        src/limitless/particle_system/effect_compiler.cpp
        src/limitless/serialization/effect_serializer.cpp
        src/limitless/serialization/emitter_serializer.cpp
        src/limitless/serialization/module_serializer.cpp
        src/limitless/serialization/distribution_serializer.cpp
        src/limitless/loaders/effect_loader.cpp
   )

set(ENGINE_SRC
        ${ENGINE_CORE}
        ${ENGINE_MATERIAL_SYSTEM}
        ${ENGINE_PARTICLE_SYSTEM}

        src/limitless/loaders/texture_loader.cpp
        src/limitless/models/skeletal_model.cpp
        src/limitless/camera.cpp
        src/limitless/loaders/model_loader.cpp
        src/limitless/assets.cpp
        src/limitless/instances/model_instance.cpp
        src/limitless/scene.cpp
        src/limitless/models/elementary_model.cpp
        src/limitless/instances/skeletal_instance.cpp
        src/limitless/instances/mesh_instance.cpp
        src/limitless/shader_storage.cpp
        src/limitless/instances/effect_instance.cpp
        src/limitless/postprocessing.cpp
        src/limitless/render.cpp
        src/limitless/scene_data.cpp
        src/limitless/skybox.cpp
        src/limitless/instances/abstract_instance.cpp
        src/limitless/instances/effect_attachable.cpp
        src/limitless/instances/light_attachable.cpp
        src/limitless/instances/elementary_instance.cpp
        src/limitless/lighting/lighting.cpp
        src/limitless/lighting/light_container.cpp
        src/limitless/loaders/threaded_model_loader.cpp
        src/limitless/util/thread_pool.cpp
        src/limitless/loaders/asset_loader.cpp
        src/limitless/font_atlas.cpp
        src/limitless/instances/text_instance.cpp
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
        "tests/uniform_serializer_test.cpp"
        "tests/material_serializer_test.cpp"
        "tests/bytebuffer_test.cpp"
        "tests/effect_serializer_test.cpp"
        "tests/material_loader_test.cpp")