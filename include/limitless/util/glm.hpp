#pragma once

namespace Limitless {
    inline glm::mat4 convert(const aiMatrix4x4& aimat) noexcept {
        return glm::mat4(aimat.a1, aimat.b1, aimat.c1, aimat.d1,
                         aimat.a2, aimat.b2, aimat.c2, aimat.d2,
                         aimat.a3, aimat.b3, aimat.c3, aimat.d3,
                         aimat.a4, aimat.b4, aimat.c4, aimat.d4);
    }

    inline glm::vec3 convert3f(const aiVector3D& aivec) noexcept {
        return glm::vec3(aivec.x, aivec.y, aivec.z);
    }

    inline glm::vec2 convert2f(const aiVector3D& aivec) noexcept {
        return glm::vec2(aivec.x, aivec.y);
    }

    inline glm::fquat convertQuat(const aiQuaternion& aiquat) noexcept {
        return { aiquat.w, aiquat.x, aiquat.y, aiquat.z };
    }
}