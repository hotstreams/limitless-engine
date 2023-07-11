#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"

#include <limitless/core/context.hpp>
#include <limitless/core/shader/shader_compiler.hpp>

using namespace Limitless;
using namespace LimitlessTest;

TEST_CASE("Manual shader program compilation") {
    Context context = Context{"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        RenderSettings settings;

        ShaderCompiler shader_compiler = ShaderCompiler(context, settings);

        shader_compiler << Shader{"../test/limitless/assets/sc1.vert", Shader::Type::Vertex}
                        << Shader{"../test/limitless/assets/sc1.frag", Shader::Type::Fragment};

        shader_compiler.compile();

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Auto shader program compilation") {
    Context context = Context{"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        RenderSettings settings;

        ShaderCompiler shader_compiler = ShaderCompiler(context, settings);

        shader_compiler.compile("../test/limitless/assets/sc1");

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Auto shader program compilation with specified actions") {
    Context context = Context{"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        RenderSettings settings;

        ShaderCompiler shader_compiler = ShaderCompiler(context, settings);

        ShaderCompiler::ShaderAction action = [] (Shader& shader) {
            shader.replaceKey("declaration", "int a = 42;");
        };

        shader_compiler.compile("../test/limitless/assets/sc2", action);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Throw on not existing path") {
    Context context = Context{"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        RenderSettings settings;

        ShaderCompiler shader_compiler = ShaderCompiler(context, settings);

        REQUIRE_THROWS_AS(shader_compiler.compile("../test/limitless/assets/empty"), shader_compilation_error);

        check_opengl_state();
    }

    check_opengl_state();
}
