#pragma once

#include <functional>

namespace Limitless {
    class Instance;
    class Camera;

    struct FrontToBackSorter {
        const Camera& camera;
        bool operator()(const std::reference_wrapper<Instance>& lhs, const std::reference_wrapper<Instance>& rhs) const noexcept;
    };

    struct BackToFrontSorter {
        const Camera& camera;
        bool operator()(const std::reference_wrapper<Instance>& lhs, const std::reference_wrapper<Instance>& rhs) const noexcept;
    };
}