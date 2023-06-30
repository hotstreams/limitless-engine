#include <limitless/core/buffer/buffer_binding_point.hpp>

using namespace Limitless;

bool Limitless::operator<(const BindingPoint& a, const BindingPoint& b) noexcept {
    return (a.target == b.target) ? a.point < b.point : a.target < b.target;
}