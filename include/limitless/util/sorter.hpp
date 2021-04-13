#pragma once

namespace LimitlessEngine {
    class AbstractInstance;
    class Camera;

    struct FrontToBackSorter {
        const Camera& camera;
        bool operator()(const AbstractInstance* lhs, const AbstractInstance* rhs) const noexcept;
    };

    struct BackToFrontSorter {
        const Camera& camera;
        bool operator()(const AbstractInstance* lhs, const AbstractInstance* rhs) const noexcept;
    };
}