#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"

#include <limitless/core/context.hpp>

using namespace Limitless;
using namespace LimitlessTest;

TEST_CASE("Context constructor") {
    Context context = {"Title", {1024, 1024}, {{WindowHint::Visible, false}}};

    REQUIRE(static_cast<GLFWwindow*>(context) != nullptr);

    check_opengl_state();
}

TEST_CASE("Context move constructor") {
    Context context1 = {"Title1", {1024, 1024}, {{WindowHint::Visible, false}}};
    Context context2 = {std::move(context1)};

    REQUIRE(context2.getSize() == glm::uvec2{1024, 1024});
    REQUIRE(static_cast<GLFWwindow*>(context2) != nullptr);

    REQUIRE(context1.getSize() == glm::uvec2{1, 1});
    REQUIRE(static_cast<GLFWwindow*>(context1) == nullptr);

    check_opengl_state();
}

TEST_CASE("Context move operator") {
    Context context1 = {"Title1", {1024, 1024}, {{WindowHint::Visible, false}}};
    Context context2 = {"Title2", {512, 512}, {{WindowHint::Visible, false}}};

    context2 = {std::move(context1)};

    REQUIRE(context2.getSize() == glm::uvec2{1024, 1024});
    REQUIRE(static_cast<GLFWwindow*>(context2) != nullptr);

    REQUIRE(context1.getSize() == glm::uvec2{512, 512});
    REQUIRE(static_cast<GLFWwindow*>(context1) != nullptr);

    check_opengl_state();
}