#include <limitless/ms/unique_material.hpp>

#include <tuple>

bool Limitless::ms::operator<(const UniqueMaterial& lhs, const UniqueMaterial& rhs) noexcept {
    return std::tie(lhs.properties, lhs.shading) < std::tie(rhs.properties, rhs.shading);
}