#include <limitless/util/sorter.hpp>

#include <limitless/instances/instance.hpp>
#include <limitless/camera.hpp>

using namespace Limitless;

bool FrontToBackSorter::operator()(const std::reference_wrapper<Instance>& lhs, const std::reference_wrapper<Instance>& rhs) const noexcept {
    const auto& a_pos = lhs.get().getPosition();
    const auto& b_pos = rhs.get().getPosition();
    const auto& c_pos = camera.getPosition();

    return glm::distance(c_pos, a_pos) < glm::distance(c_pos, b_pos);
}


bool BackToFrontSorter::operator()(const std::reference_wrapper<Instance>& lhs, const std::reference_wrapper<Instance>& rhs) const noexcept {
    const auto& a_pos = lhs.get().getPosition();
    const auto& b_pos = rhs.get().getPosition();
    const auto& c_pos = camera.getPosition();

    return glm::distance(c_pos, a_pos) > glm::distance(c_pos, b_pos);
}