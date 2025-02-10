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

bool boxInFrustum(const Frustum& frustum, const Box& box)
{
    auto min = box.center - box.size * 0.5f;
    auto max = box.center + box.size * 0.5f;

    // check box outside/inside of frustum
    for( int i=0; i<6; i++ )
    {
        int out = 0;
        out += ((dot( frustum.planes[i], glm::vec4(min.x, min.y, min.z, 1.0f) ) < 0.0 )?1:0);
        out += ((dot( frustum.planes[i], glm::vec4(max.x, min.y, min.z, 1.0f) ) < 0.0 )?1:0);
        out += ((dot( frustum.planes[i], glm::vec4(min.x, max.y, min.z, 1.0f) ) < 0.0 )?1:0);
        out += ((dot( frustum.planes[i], glm::vec4(max.x, max.y, min.z, 1.0f) ) < 0.0 )?1:0);
        out += ((dot( frustum.planes[i], glm::vec4(min.x, min.y, max.z, 1.0f) ) < 0.0 )?1:0);
        out += ((dot( frustum.planes[i], glm::vec4(max.x, min.y, max.z, 1.0f) ) < 0.0 )?1:0);
        out += ((dot( frustum.planes[i], glm::vec4(min.x, max.y, max.z, 1.0f) ) < 0.0 )?1:0);
        out += ((dot( frustum.planes[i], glm::vec4(max.x, max.y, max.z, 1.0f) ) < 0.0 )?1:0);
        if( out==8 ) return false;
    }

    // check frustum outside/inside box
    int out;
    out=0; for( int i=0; i<8; i++ ) out += ((frustum.points[i].x > max.x)?1:0); if( out==8 ) return false;
    out=0; for( int i=0; i<8; i++ ) out += ((frustum.points[i].x < min.x)?1:0); if( out==8 ) return false;
    out=0; for( int i=0; i<8; i++ ) out += ((frustum.points[i].y > max.y)?1:0); if( out==8 ) return false;
    out=0; for( int i=0; i<8; i++ ) out += ((frustum.points[i].y < min.y)?1:0); if( out==8 ) return false;
    out=0; for( int i=0; i<8; i++ ) out += ((frustum.points[i].z > max.z)?1:0); if( out==8 ) return false;
    out=0; for( int i=0; i<8; i++ ) out += ((frustum.points[i].z < min.z)?1:0); if( out==8 ) return false;

    return true;
}


bool Frustum::intersects(Instance& instance) const {
    //return intersects(instance.getBoundingBox());
    return boxInFrustum(*this, instance.getBoundingBox());
}
