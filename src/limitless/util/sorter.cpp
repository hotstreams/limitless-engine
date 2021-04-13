#include <limitless/util/sorter.hpp>

#include <limitless/instances/abstract_instance.hpp>
#include <limitless/camera.hpp>

using namespace LimitlessEngine;

bool FrontToBackSorter::operator()(const AbstractInstance* lhs, const AbstractInstance* rhs) const noexcept {
    const auto& a_pos = lhs->getPosition();
    const auto& b_pos = rhs->getPosition();
    const auto& c_pos = camera.getPosition();

    return glm::distance(c_pos, a_pos) < glm::distance(c_pos, b_pos);
}


bool BackToFrontSorter::operator()(const AbstractInstance* lhs, const AbstractInstance* rhs) const noexcept {
    const auto& a_pos = lhs->getPosition();
    const auto& b_pos = rhs->getPosition();
    const auto& c_pos = camera.getPosition();

    return glm::distance(c_pos, a_pos) > glm::distance(c_pos, b_pos);
}