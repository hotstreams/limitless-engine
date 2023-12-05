#########################################
cmake_minimum_required(VERSION 3.10)

#########################################
project(limitless-engine-tests)

if (NOT BUILD_TESTS)
    return()
endif()

add_executable(limitless-tests
    limitless/catch_amalgamated.cpp

    limitless/core/shader_compiler_test.cpp
    limitless/core/shader_test.cpp
    limitless/core/context_test.cpp
    limitless/core/state_buffer_test.cpp
    limitless/core/state_texture_test.cpp
    limitless/core/texture_builder_test.cpp
    limitless/ms/material_builder_test.cpp
    limitless/ms/material_test.cpp
    limitless/ms/material_compiler_test.cpp
#    limitless/instance/model_instance_test.cpp
#    limitless/instance/skeletal_instance_test.cpp
#    limitless/instance/instance_attachment_test.cpp
#    limitless/instance/socket_attachment_test.cpp
)

target_link_libraries(limitless-tests PRIVATE limitless-engine)
target_compile_definitions(limitless-tests PRIVATE LIMITLESS_OPENGL_DEBUG)