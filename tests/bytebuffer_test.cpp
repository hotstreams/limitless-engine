#include "catch_amalgamated.hpp"

#include <util/bytebuffer.hpp>

using namespace Limitless;

TEST_CASE("bytebuffer") {
    ByteBuffer buffer;

    std::string test{"shrek!"}, test1{};
    float f = 15.3f;
    int i = 1;

    buffer << test << f << test1 << i;

    std::string test2, test3;
    float f1;
    int i1;

    buffer >> test2 >> f1 >> test3 >> i1;

    REQUIRE(test == test2);
    REQUIRE(test1 == test3);
    REQUIRE(f == f1);
    REQUIRE(i == i1);
}
