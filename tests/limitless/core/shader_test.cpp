#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"

#include <limitless/core/context.hpp>
#include <limitless/core/shader/shader.hpp>

using namespace Limitless;
using namespace LimitlessTest;

TEST_CASE("Loads simple shader without error") {
    Context context = {"Title", {512, 512}, nullptr, {{WindowHint::Hint::Visible, false}}};

    {
        Shader shader = Shader{"../../tests/limitless/assets/shader1.vert", Shader::Type::Vertex};

        REQUIRE(shader.getId() != 0);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Replaces version in shader") {
    Context context = {"Title", {512, 512}, nullptr, {{WindowHint::Hint::Visible, false}}};

    {
        Shader shader = Shader{"../../tests/limitless/assets/shader2.vert", Shader::Type::Vertex};

        REQUIRE(shader.getId() != 0);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Resolves include in shader") {
    Context context = {"Title", {512, 512}, nullptr, {{WindowHint::Hint::Visible, false}}};

    {
        Shader shader = Shader{"../../tests/limitless/assets/shader4.vert", Shader::Type::Vertex};

        REQUIRE(shader.getId() != 0);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Replaces key-line in shader") {
    Context context = {"Title", {512, 512}, nullptr, {{WindowHint::Hint::Visible, false}}};

    {
        Shader shader = Shader{"../../tests/limitless/assets/shader6.vert", Shader::Type::Vertex};

        REQUIRE(shader.getId() != 0);

        shader.replaceKey("SOME_STATEMENT", "int a = 10;");

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Throw file not found") {
    Context context = {"Title", {512, 512}, nullptr, {{WindowHint::Hint::Visible, false}}};

    {
        REQUIRE_THROWS_AS(Shader("../../tests/limitless/assets/3228.vert", Shader::Type::Vertex), shader_file_not_found);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Throw include not found") {
    Context context = {"Title", {512, 512}, nullptr, {{WindowHint::Hint::Visible, false}}};

    {
        REQUIRE_THROWS_AS(Shader("../../tests/limitless/assets/shader7.vert", Shader::Type::Vertex), shader_include_not_found);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Throw shader_compilation_error") {
    Context context = {"Title", {512, 512}, nullptr, {{WindowHint::Hint::Visible, false}}};

    {
        Shader shader = Shader{"../../tests/limitless/assets/shader8.vert", Shader::Type::Vertex};

        REQUIRE_THROWS_AS(shader.compile(), shader_compilation_error);

        // we should reset state because shader compilation got an error
        gl_error_count = 0;
    }
}

TEST_CASE("Compiles simple shader") {
    Context context = {"Title", {512, 512}, nullptr, {{WindowHint::Hint::Visible, false}}};

    {
        Shader shader = Shader{"../../tests/limitless/assets/shader1.vert", Shader::Type::Vertex};

        REQUIRE_NOTHROW(shader.compile());

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Shader move ctor") {
    Context context = {"Title", {512, 512}, nullptr, {{WindowHint::Hint::Visible, false}}};

    {
        Shader shader1 = Shader{"../../tests/limitless/assets/shader1.vert", Shader::Type::Vertex};
        Shader shader2 = std::move(shader1);

        REQUIRE(shader1.getId() == 0);
        REQUIRE(shader2.getId() != 0);

        check_opengl_state();
    }

    check_opengl_state();
}
