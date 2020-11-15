cmake_minimum_required(VERSION 3.10)
project(engine)

set(CMAKE_CXX_STANDARD 17)

#set(EXTRA_WARNINGS "-Wnon-virtual-dtor -Wold-style-cast -Wunused -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2 -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast")

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
    link_libraries(GLEW glfw GL assimp freetype)
endif()

set(ENGINE_CORE
        src/core/bindless_texture.cpp
        src/core/buffer_builder.cpp
        src/core/context.cpp
        src/core/context_debug.cpp
        src/core/context_initializer.cpp
        src/core/context_observer.cpp
        src/core/context_state.cpp
        src/core/shader_compiler.cpp
        src/core/shader_program.cpp
        src/core/uniform.cpp
        src/core/texture_binder.cpp
        src/core/vertex_array.cpp
        src/core/framebuffer.cpp
        src/core/state_buffer.cpp
        src/core/named_buffer.cpp
        src/core/triple_buffer.cpp
        src/core/indexed_buffer.cpp
        src/core/shader.cpp
        src/core/state_texture.cpp
        src/core/named_texture.cpp
        src/core/context_thread_pool.cpp
        src/core/mutable_texture.cpp
        src/core/immutable_texture.cpp
   )

set(ENGINE_MATERIAL_SYSTEM
        src/material_system/properties.cpp
        src/material_system/material.cpp
        src/material_system/material_builder.cpp
        src/material_system/custom_material.cpp
        src/material_system/custom_material_builder.cpp
        src/material_system/material_compiler.cpp
        src/material_system/material_instance.cpp
    )

set(ENGINE_PARTICLE_SYSTEM
        src/particle_system/emitter.cpp
        src/particle_system/emitter_module.cpp
        src/particle_system/sprite_emitter.cpp
        src/particle_system/mesh_emitter.cpp
        src/particle_system/effect_renderer.cpp
        src/particle_system/emitter_renderer.cpp
        src/particle_system/emitter_spawn.cpp
        src/particle_system/emitter_visitor.cpp
        src/particle_system/unique_emitter.cpp
        src/particle_system/effect_builder.cpp
        src/particle_system/effect_compiler.cpp
   )

set(ENGINE_SRC
        ${ENGINE_CORE}
        ${ENGINE_MATERIAL_SYSTEM}
        ${ENGINE_PARTICLE_SYSTEM}

        src/texture_loader.cpp
        src/skeletal_model.cpp
        src/camera.cpp
        src/model_loader.cpp
        src/assets.cpp
        src/model_instance.cpp
        src/scene.cpp
        src/elementary_model.cpp
        src/skeletal_instance.cpp
        src/mesh_instance.cpp
        src/shader_storage.cpp
        src/effect_instance.cpp
        src/postprocessing.cpp
        src/render.cpp
        src/scene_data.cpp
        src/skybox.cpp
        src/abstract_instance.cpp
        src/effect_attachable.cpp
        src/light_attachable.cpp
        src/elementary_instance.cpp
        src/lighting.cpp
        src/light_container.cpp
        src/threaded_model_loader.cpp
        src/util/thread_pool.cpp
        src/asset_loader.cpp
        src/font_atlas.cpp
        src/text_instance.cpp
        src/util/bytebuffer.cpp
        src/util/bytereader.cpp
        src/util/bytewriter.cpp
    )

set(TEST_SRC tests/test_main.cpp tests/test_serialization.cpp)

#file(GLOB list RELATIVE ${CMAKE_SOURCE_DIR} "demo/*.cpp")
#foreach(file ${list})
#    string(REPLACE ".cpp" "" exec_name ${file})
#    get_filename_component(filename ${exec_name} NAME)
#    add_executable(${filename} ${ENGINE_SRC} ${file})
#endforeach()
add_executable(demo ${ENGINE_SRC} "demo/demo.cpp")
add_executable(tests ${ENGINE_SRC} ${TEST_SRC})
