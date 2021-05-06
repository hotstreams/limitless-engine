#pragma once

#include <functional>

namespace Limitless {
    class AbstractInstance;
    class Camera;

    struct FrontToBackSorter {
        const Camera& camera;
        bool operator()(const std::reference_wrapper<AbstractInstance>& lhs, const std::reference_wrapper<AbstractInstance>& rhs) const noexcept;
    };

    struct BackToFrontSorter {
        const Camera& camera;
        bool operator()(const std::reference_wrapper<AbstractInstance>& lhs, const std::reference_wrapper<AbstractInstance>& rhs) const noexcept;
    };
}