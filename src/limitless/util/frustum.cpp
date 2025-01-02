#include <limitless/util/frustum.hpp>
#include <limitless/renderer/shader_type.hpp>

using namespace Limitless;

template<Frustum::Planes i, Frustum::Planes j>
struct ij2k { enum { k = (int)i * (9 - (int)i) / 2 + (int)j - 1 }; };

template <Frustum::Planes a, Frustum::Planes b, Frustum::Planes c>
glm::vec3 Frustum::intersection(const std::vector<glm::vec3>& crosses) const {
    auto dot = glm::dot(glm::vec3(planes[a]), crosses[ij2k<b, c>::k]);
    auto r = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) * glm::vec3(planes[a].w, planes[b].w, planes[c].w);
    return r * (-1.0f / dot);
}

Frustum::Frustum(const glm::mat4& matrix) 
    : planes {}
    , points {}
{
    auto m = glm::transpose(matrix);
    planes[0] = m[3] + m[0];
    planes[1] = m[3] - m[0];
    planes[2] = m[3] + m[1];
    planes[3] = m[3] - m[1];
    planes[4] = m[3] + m[2];
    planes[5] = m[3] - m[2];

    std::vector<glm::vec3> combinations = {
        glm::cross(glm::vec3(planes[Left]),   glm::vec3(planes[Right])),
        glm::cross(glm::vec3(planes[Left]),   glm::vec3(planes[Bottom])),
        glm::cross(glm::vec3(planes[Left]),   glm::vec3(planes[Top])),
        glm::cross(glm::vec3(planes[Left]),   glm::vec3(planes[Near])),
        glm::cross(glm::vec3(planes[Left]),   glm::vec3(planes[Far])),
        glm::cross(glm::vec3(planes[Right]),  glm::vec3(planes[Bottom])),
        glm::cross(glm::vec3(planes[Right]),  glm::vec3(planes[Top])),
        glm::cross(glm::vec3(planes[Right]),  glm::vec3(planes[Near])),
        glm::cross(glm::vec3(planes[Right]),  glm::vec3(planes[Far])),
        glm::cross(glm::vec3(planes[Bottom]), glm::vec3(planes[Top])),
        glm::cross(glm::vec3(planes[Bottom]), glm::vec3(planes[Near])),
        glm::cross(glm::vec3(planes[Bottom]), glm::vec3(planes[Far])),
        glm::cross(glm::vec3(planes[Top]),    glm::vec3(planes[Near])),
        glm::cross(glm::vec3(planes[Top]),    glm::vec3(planes[Far])),
        glm::cross(glm::vec3(planes[Near]),   glm::vec3(planes[Far]))
    };

    points[0] = intersection<Left,  Bottom, Near>(combinations);
    points[1] = intersection<Left,  Top,    Near>(combinations);
    points[2] = intersection<Right, Bottom, Near>(combinations);
    points[3] = intersection<Right, Top,    Near>(combinations);
    points[4] = intersection<Left,  Bottom, Far>(combinations);
    points[5] = intersection<Left,  Top,    Far>(combinations);
    points[6] = intersection<Right, Bottom, Far>(combinations);
    points[7] = intersection<Right, Top,    Far>(combinations);
}

bool Frustum::intersects(const Box& box) const {
    auto min = box.center - box.size * 0.5f;
    auto max = box.center + box.size * 0.5f;

    for (const auto& plane: planes) {
        if ((glm::dot(plane, glm::vec4(min.x, min.y, min.z, 1.0f)) < 0.0) &&
            (glm::dot(plane, glm::vec4(max.x, min.y, min.z, 1.0f)) < 0.0) &&
            (glm::dot(plane, glm::vec4(min.x, max.y, min.z, 1.0f)) < 0.0) &&
            (glm::dot(plane, glm::vec4(max.x, max.y, min.z, 1.0f)) < 0.0) &&
            (glm::dot(plane, glm::vec4(min.x, min.y, max.z, 1.0f)) < 0.0) &&
            (glm::dot(plane, glm::vec4(max.x, min.y, max.z, 1.0f)) < 0.0) &&
            (glm::dot(plane, glm::vec4(min.x, max.y, max.z, 1.0f)) < 0.0) &&
            (glm::dot(plane, glm::vec4(max.x, max.y, max.z, 1.0f)) < 0.0))
        {
            return false;
        }
    }

    int out;
    out = 0;
    for (int i = 0; i<8; i++) out += ((points[i].x > max.x) ? 1 : 0); if (out == 8) return false;
    out = 0;
    for (int i = 0; i<8; i++) out += ((points[i].x < min.x) ? 1 : 0); if (out == 8) return false;
    out = 0;
    for (int i = 0; i<8; i++) out += ((points[i].y > max.y) ? 1 : 0); if (out == 8) return false;
    out = 0;
    for (int i = 0; i<8; i++) out += ((points[i].y < min.y) ? 1 : 0); if (out == 8) return false;
    out = 0;
    for (int i = 0; i<8; i++) out += ((points[i].z > max.z) ? 1 : 0); if (out == 8) return false;
    out = 0;
    for (int i = 0; i<8; i++) out += ((points[i].z < min.z) ? 1 : 0); if (out == 8) return false;

    return true;
}

Frustum Frustum::fromCamera(const Camera& camera) {
    return Frustum {camera.getProjection() * camera.getView()};
}


bool test(const Frustum& frustum, const Box& box) {
    // The main intersection routine assumes multiples of 8 items
    static constexpr size_t MODULO = 4u;
    glm::vec3 centers[MODULO] = {};
    glm::vec3 extents[MODULO] = {};
    uint16_t results[MODULO];

    centers[0] = box.center;
    extents[0] = box.size * 0.5f;

    size_t count = MODULO;

    for (size_t i = 0; i < count; i++) {
        int visible = ~0;
        for (size_t j = 0; j < 6; j++) {
            const float dot =
                    frustum.planes[j].x * centers[i].x - std::abs(frustum.planes[j].x) * extents[i].x +
                    frustum.planes[j].y * centers[i].y - std::abs(frustum.planes[j].y) * extents[i].y +
                    frustum.planes[j].z * centers[i].z - std::abs(frustum.planes[j].z) * extents[i].z +
                    frustum.planes[j].w;

            visible &= std::signbit(dot);
        }

        auto r = results[i];
        r &= ~uint16_t(1u);
        r |= uint16_t(visible);
        results[i] = r;
    }

    return bool(results[0] & 1);
}

bool intersectsFrustumAABB(const Frustum& frustum, const Box& box) {
    // Calculate the min and max corners of the AABB
    glm::vec3 halfSize = box.size * 0.5f;
    glm::vec3 minCorner = box.center - halfSize;
    glm::vec3 maxCorner = box.center + halfSize;

    // Check each plane of the frustum
    for (const auto& plane : frustum.planes) {
        glm::vec3 planeNormal(plane.x, plane.y, plane.z);
        float planeOffset = plane.w;

        // Test all 8 corners of the AABB against this plane
        bool isOutside = true;

        // Loop through all 8 corners
        for (int i = 0; i < 8; ++i) {
            glm::vec3 corner = glm::vec3(
                    (i & 1) ? maxCorner.x : minCorner.x,
                    (i & 2) ? maxCorner.y : minCorner.y,
                    (i & 4) ? maxCorner.z : minCorner.z
            );

            // If at least one corner is inside the plane, the box is not fully outside
            if (glm::dot(planeNormal, corner) + planeOffset >= 0) {
                isOutside = false;
                break;
            }
        }

        // If all corners are outside this plane, the box is completely outside the frustum
        if (isOutside) {
            return false;
        }
    }

    // If no plane fully excludes the box, it intersects or is inside the frustum
    return true;
}


bool Frustum::intersects(Instance& instance) const {
    return intersects(instance.getBoundingBox());
//    return intersectsFrustumAABB(*this, instance.getBoundingBox());
}
