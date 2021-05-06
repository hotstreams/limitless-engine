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

    inline glm::vec3 flipYZ(const glm::vec3& v) noexcept {
        return { v.x, v.z, v.y };
    }

    inline glm::fquat flipYZ(const glm::fquat& q) noexcept {
        return { -q.w, q.x, q.z, q.y };
    }

    inline glm::mat4 flipYZ(const glm::mat4& m) noexcept {
        const glm::mat4 rot = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
        return rot * m;
    }

    inline glm::mat4 flipYZTransformationMatrix(const glm::mat4& m) noexcept {
        glm::vec3 translation = { m[0][3], m[1][3], m[2][3] };
        // TODO: fix for negatives
        glm::vec3 scale = { glm::length(glm::vec3{m[0][0], m[1][0], m[2][0]}),
                            glm::length(glm::vec3{m[0][1], m[1][1], m[2][1]}),
                            glm::length(glm::vec3{m[0][2], m[1][2], m[2][2]}) };
        glm::mat4 rotation = {
                m[0][0] / scale.x, m[0][1] / scale.y, m[0][2] / scale.z, 0.0f,
                m[1][0] / scale.x, m[1][1] / scale.y, m[1][2] / scale.z, 0.0f,
                m[2][0] / scale.x, m[2][1] / scale.y, m[2][2] / scale.z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,
        };

        translation = flipYZ(translation);
        scale = flipYZ(scale);
        rotation = flipYZ(rotation);

        return glm::translate(glm::mat4{1.0f}, translation) * rotation * glm::scale(glm::mat4{1.0f}, scale);
    }

    inline glm::fquat convertQuat(const aiQuaternion& aiquat) noexcept {
        return { aiquat.w, aiquat.x, aiquat.y, aiquat.z };
    }
}