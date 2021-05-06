#pragma once

#include <limitless/ms/property.hpp>
#include <limitless/ms/shading.hpp>

#include <set>

namespace Limitless::ms {
    struct UniqueMaterial {
        std::set<Property> properties;
        Shading shading;
    };
    bool operator<(const UniqueMaterial&, const UniqueMaterial&) noexcept;
}