#include "catch_amalgamated.hpp"

#include <limitless/core/uniform_serializer.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/util/bytebuffer.hpp>

using namespace LimitlessEngine;

TEST_CASE("serialize UniformValue") {
    UniformSerializer serializer;

    UniformValue<float> uniform{"test_uniform", 13.5f};

    auto buffer = serializer.serialize(uniform);

    auto deserialized_uniform = serializer.deserialize(buffer);

    REQUIRE(deserialized_uniform->getName() == "test_uniform");
    REQUIRE_NOTHROW(dynamic_cast<UniformValue<float>&>(*deserialized_uniform));
    REQUIRE(deserialized_uniform->getValueType() == UniformValueType::Float);
    REQUIRE(dynamic_cast<UniformValue<float>&>(*deserialized_uniform).getValue() == 13.5f);
}