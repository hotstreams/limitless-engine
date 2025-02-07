#pragma once

#include <limitless/models/mesh.hpp>

namespace Limitless {
    class GeoClipMap {
        static inline int _patch_2d(const int x, const int y, const int res);
        static std::shared_ptr<AbstractMesh> _create_mesh(
            std::vector<VertexNormalTangent>&& p_vertices,
            std::vector<uint32_t>&& p_indices,
            std::string name
        );
    public:
        enum MeshType {
            TILE,
            FILLER,
            TRIM,
            CROSS,
            SEAM
        };

        static std::vector<std::shared_ptr<AbstractMesh>> generate(const int p_resolution, const int p_clipmap_levels);
    };

    inline int GeoClipMap::_patch_2d(const int x, const int y, const int res) {
        return y * res + x;
    }
}