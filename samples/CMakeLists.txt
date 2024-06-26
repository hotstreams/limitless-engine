#########################################
cmake_minimum_required(VERSION 3.10)

#########################################
project(limitless-samples)

if (NOT BUILD_SAMPLES)
    return()
endif()

# gltf model viewer
add_executable(limitless-gltf-viewer
    gltf_viewer/gltf_viewer.cpp
)
target_link_libraries(limitless-gltf-viewer PRIVATE limitless-engine)

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