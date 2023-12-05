#########################################
cmake_minimum_required(VERSION 3.10)

#########################################
project(limitless-engine-samples)

if (NOT BUILD_SAMPLES)
    return()
endif()

add_executable(limitless-demo
    demo/demo.cpp
    #    demo/materials_demoscene.cpp
    #    demo/lighting_demoscene.cpp
    #    demo/effects_demoscene.cpp
    #    demo/models_demoscene.cpp
    demo/assets.cpp
)
target_link_libraries(limitless-demo PRIVATE limitless-engine)

add_executable(limitless-gltf-viewer
    demo/gltf_viewer/gltf_viewer.cpp
)
target_link_libraries(limitless-gltf-viewer PRIVATE limitless-engine)