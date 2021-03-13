#include "catch_amalgamated.hpp"

#include <core/context.hpp>
#include <material_system/material_loader.hpp>
#include <material_system/material_builder.hpp>
#include <iostream>
#include <asset_loader.hpp>
using namespace LimitlessEngine;

class FakeBackend {
private:
    Context ctx;
public:
    FakeBackend() : ctx{"test", {1, 1}, {{WindowHint::Visible, false}}} {

    }
};

TEST_CASE("material loader") {
    FakeBackend fake;

    MaterialLoader loader;

std::cout << "dir = " << MATERIAL_DIR << std::endl;
//    MaterialBuilder builder;
//    auto mat = builder  .create("test")
//                        .add(PropertyType::Color, {1.0f, 0.5f, 0.5f, 1.0f})
//                        .setBlending(Blending::Additive)
//                        .build();
//
//    loader.save("test");

    loader.load("test");
}