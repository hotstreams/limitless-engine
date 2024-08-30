#include "limitless/catch_amalgamated.hpp"

#include "limitless/opengl_state.hpp"

#include <limitless/core/context.hpp>

using namespace Limitless;

TEST_CASE("StateTexture id generation") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};

    StateTexture texture;
    REQUIRE(texture.getId() != 0);

    check_opengl_state();
}

TEST_CASE("StateTexture cleaning up ContextState") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};

    constexpr auto bind_index = 3;
    {
        StateTexture texture;
        texture.bind(GL_TEXTURE_2D, bind_index);
    }

    REQUIRE(context.getTextureBound().at(bind_index) == 0);

    check_opengl_state();
}

TEST_CASE("StateTexture move constructor") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};

    StateTexture texture;
    const auto id = texture.getId();

    StateTexture moved_texture {std::move(texture)};

    REQUIRE(moved_texture.getId() == id);
    REQUIRE(texture.getId() == 0);

    check_opengl_state();
}

TEST_CASE("StateTexture move operator") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};

    StateTexture texture;
    const auto id = texture.getId();

    StateTexture moved_texture;
    const auto moved_id = moved_texture.getId();
    moved_texture = std::move(texture);

    REQUIRE(moved_texture.getId() == id);
    REQUIRE(texture.getId() == moved_id);

    check_opengl_state();
}

TEST_CASE("StateTexture clone function") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};

    auto texture = std::unique_ptr<ExtensionTexture>(new StateTexture());

    auto* new_texture = texture->clone();

    REQUIRE(new_texture->getId() != texture->getId());
    REQUIRE_NOTHROW(dynamic_cast<StateTexture&>(*new_texture));

    check_opengl_state();
}

TEST_CASE("StateTexture activate") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};

    constexpr auto active_index = 5;
    StateTexture::activate(active_index);

    REQUIRE(context.getActiveTexture() == active_index);
    REQUIRE_THROWS(StateTexture::activate(ContextInitializer::limits.max_texture_units));

    check_opengl_state();
}

TEST_CASE("StateTexture bind") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};

    StateTexture texture;

    constexpr auto bind_index = 1;
    texture.bind(GL_TEXTURE_2D, bind_index);

    REQUIRE(context.getActiveTexture() == bind_index);
    REQUIRE(context.getTextureBound().at(bind_index) == texture.getId());
    REQUIRE_THROWS(texture.bind(GL_TEXTURE_2D, ContextInitializer::limits.max_texture_units));

    check_opengl_state();
}

