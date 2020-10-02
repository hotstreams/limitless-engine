cmake_minimum_required(VERSION 3.10)
project(engine)

set(CMAKE_CXX_STANDARD 17)

#set(EXTRA_WARNINGS "-Wnon-virtual-dtor -Wold-style-cast -Wunused -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2 -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast")
set(CMAKE_CXX_FLAGS_DEBUG "-DGL_DEBUG -O0 -ggdb -Wpedantic -Wextra -Wall -Wshadow ${EXTRA_WARNINGS}")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

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

    #find_package(freetype REQUIRED)
    #message(STATUS "Found FREETYPE in ${FREETYPE_INCLUDE_DIRS}")
    #include_directories(${FREETYPE_INCLUDE_DIRS})
    #link_directories(${FREETYPE_LIBRARIES})

    link_libraries(opengl32 glew32 glfw3 assimp)
endif()

if (UNIX)
    link_libraries(GLEW glfw GL assimp)
endif()

set(ENGINE_SRC
        src/bindless_texture.cpp
        src/bindless_texture_storage.cpp
        src/buffer_builder.cpp
        src/context.cpp
        src/context_debug.cpp
        src/context_initializer.cpp
        src/context_observer.cpp
        src/context_state.cpp
        src/material.cpp
        src/shader_compiler.cpp
        src/shader_program.cpp
        src/uniform.cpp
        src/texture.cpp
        src/texture_binder.cpp
        src/texture_loader.cpp
        src/vertex_array.cpp
        src/material_builder.cpp
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
        src/custom_material.cpp
        src/custom_material_builder.cpp
        src/emitter.cpp
        src/emitter_module.cpp
        src/sprite_emitter.cpp
        src/mesh_emitter.cpp
        src/effect.cpp
        src/particle_system.cpp
        src/emitter_buffer.cpp
        src/framebuffer.cpp
        src/postprocessing.cpp
        src/render.cpp
        src/scene_data.cpp
        src/skybox.cpp
        src/state_buffer.cpp
        src/named_buffer.cpp
        src/triple_buffer.cpp
        src/indexed_buffer.cpp
        src/shader.cpp
        src/material_compiler.cpp
        src/material_instance.cpp)

file(GLOB list RELATIVE ${CMAKE_SOURCE_DIR} "demo/*.cpp")
foreach(file ${list})
    string(REPLACE ".cpp" "" exec_name ${file})
    get_filename_component(filename ${exec_name} NAME)
    add_executable(${filename} ${ENGINE_SRC} ${file})
endforeach()
