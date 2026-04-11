#########################################
cmake_minimum_required(VERSION 3.10)

#########################################
project(limitless-samples)

# Tiny CLI without engine deps; available even when BUILD_SAMPLES is OFF (standalone presets).
add_subdirectory(../tools/billboard_generator ${CMAKE_CURRENT_BINARY_DIR}/tools_billboard_generator)

if (NOT BUILD_SAMPLES)
    return()
endif()

# gltf model viewer
add_executable(limitless-gltf-viewer
    gltf_viewer/gltf_viewer.cpp
)
target_link_libraries(limitless-gltf-viewer PRIVATE limitless-engine)

# SpeedTree wind + glTF (Conifer default)
add_executable(limitless-speedtree
    speedtree/main.cpp
)
target_link_libraries(limitless-speedtree PRIVATE limitless-engine)

# test materials scene
add_executable(limitless-materials
    materials/main.cpp
    materials/assets.cpp
    materials/scene.cpp
)
target_link_libraries(limitless-materials PRIVATE limitless-engine)

# test effects scene
add_executable(limitless-effects
    effects/main.cpp
    effects/assets.cpp
    effects/scene.cpp
)
target_link_libraries(limitless-effects PRIVATE limitless-engine)

# test lighting scene
add_executable(limitless-lighting
    lighting/main.cpp
    lighting/assets.cpp
    lighting/scene.cpp
)
target_link_libraries(limitless-lighting PRIVATE limitless-engine)

# test terrain scene
add_executable(limitless-terrain
        terrain/main.cpp
        terrain/assets.cpp
        terrain/scene.cpp
        )
target_link_libraries(limitless-terrain PRIVATE limitless-engine)

# shadows (CSM) test scene
add_executable(limitless-shadows
    shadows/main.cpp
    shadows/assets.cpp
    shadows/scene.cpp
    shadows/debug_lines.cpp
)
target_link_libraries(limitless-shadows PRIVATE limitless-engine)

# LOD instancing test scene
add_executable(limitless-lod-instancing
    lod_instancing/main.cpp
    lod_instancing/assets.cpp
    lod_instancing/scene.cpp
)
target_link_libraries(limitless-lod-instancing PRIVATE limitless-engine)

# Skeletal mesh instancing (packed bone buffer + model_buffer)
add_executable(limitless-skeletal-lod-instancing
    skeletal_lod_instancing/main.cpp
    skeletal_lod_instancing/assets.cpp
    skeletal_lod_instancing/scene.cpp
)
target_link_libraries(limitless-skeletal-lod-instancing PRIVATE limitless-engine)

# Indirect draw (Multi-Draw Indirect) test scene
add_executable(limitless-indirect-draw
    indirect_draw/main.cpp
    indirect_draw/assets.cpp
    indirect_draw/scene.cpp
)
target_link_libraries(limitless-indirect-draw PRIVATE limitless-engine)

# Geometry pool (shared VBO/EBO) without MDI — regression / visual test
add_executable(limitless-optimization-test
    optimization_test/main.cpp
    optimization_test/assets.cpp
    optimization_test/scene.cpp
)
target_link_libraries(limitless-optimization-test PRIVATE limitless-engine)

# SSAO visual / parameter test (deferred, runtime toggle + UBO tweaks)
add_executable(limitless-ssao-test
    ssao_test/main.cpp
    ssao_test/assets.cpp
    ssao_test/scene.cpp
)
target_link_libraries(limitless-ssao-test PRIVATE limitless-engine)

# FXAA postprocess test (deferred, toggle pass + uniform tuning)
add_executable(limitless-postprocess-test
    postprocess/main.cpp
    postprocess/assets.cpp
    postprocess/scene.cpp
)
target_link_libraries(limitless-postprocess-test PRIVATE limitless-engine)