#pragma once

#include <limitless/util/box.hpp>
#include <limitless/camera.hpp>
#include <array>
#include <limitless/instances/instance.hpp>

namespace Limitless {
    class Frustum {
    public:
        enum Planes { Left, Right, Bottom, Top, Near, Far };
    private:
        std::array<glm::vec4, 6> planes;
        std::array<glm::vec3, 8> points;

        template <Frustum::Planes a, Frustum::Planes b, Frustum::Planes c>
        [[nodiscard]] glm::vec3 intersection(const std::vector<glm::vec3>& crosses) const;

        /**
         * Creates frustum from [projection * view] matrix
         */
        explicit Frustum(const glm::mat4& matrix);
    public:
        /*
         * Checks frustum intersection with a box
         */
        bool intersects(const Box& box) const;

        /**
         * Checks frustum intersection with an instance and prepares it for rendering in frustum
         */
        bool intersects(Instance& instance);

        /**
         * Creates Frustum from Camera
         */
        static Frustum fromCamera(const Camera& camera);
    };
}