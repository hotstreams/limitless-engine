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
    limitless/instance/lod_test.cpp
)

target_link_libraries(limitless-tests PRIVATE limitless-engine)
target_compile_definitions(limitless-tests PRIVATE LIMITLESS_OPENGL_DEBUG)

add_test(NAME limitless-tests COMMAND $<TARGET_FILE:limitless-tests>)

if (WIN32)
    # Ensure runtime DLLs are next to the test executable so CTest can run it.
    add_custom_command(TARGET limitless-tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/bin/glew32.dll"
            "$<TARGET_FILE_DIR:limitless-tests>/glew32.dll"
        VERBATIM
    )
endif()

# A smaller executable that contains ONLY the LOD tests (faster local iteration).
add_executable(limitless-lod-tests
    limitless/catch_amalgamated.cpp
    limitless/instance/lod_test.cpp
)
target_link_libraries(limitless-lod-tests PRIVATE limitless-engine)
target_compile_definitions(limitless-lod-tests PRIVATE LIMITLESS_OPENGL_DEBUG)

add_test(NAME limitless-lod-tests COMMAND $<TARGET_FILE:limitless-lod-tests>)

if (WIN32)
    add_custom_command(TARGET limitless-lod-tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/bin/glew32.dll"
            "$<TARGET_FILE_DIR:limitless-lod-tests>/glew32.dll"
        VERBATIM
    )
endif()