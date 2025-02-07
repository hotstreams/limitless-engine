#include <limitless/core/indexed_stream.hpp>
#include "limitless/util/geoclipmap.hpp"

using namespace Limitless;

std::shared_ptr<AbstractMesh> GeoClipMap::_create_mesh(
        std::vector<VertexNormalTangent>&& p_vertices,
        std::vector<uint32_t>&& p_indices,
        std::string name
) {
    auto mesh = std::make_shared<Limitless::Mesh>(
        std::make_unique<Limitless::IndexedVertexStream<Limitless::VertexNormalTangent>>(
                std::move(p_vertices),
                std::move(p_indices),
                Limitless::VertexStreamUsage::Dynamic,
                Limitless::VertexStreamDraw::Triangles
        ),
        name
    );

	return mesh;
}

std::vector<std::shared_ptr<AbstractMesh>> GeoClipMap::generate(const int p_size, const int p_levels) {
    std::shared_ptr<AbstractMesh> tile_mesh;
    std::shared_ptr<AbstractMesh> filler_mesh;
    std::shared_ptr<AbstractMesh> trim_mesh;
    std::shared_ptr<AbstractMesh> cross_mesh;
    std::shared_ptr<AbstractMesh> seam_mesh;
    std::shared_ptr<AbstractMesh> skirt_mesh;

	int TILE_RESOLUTION = p_size;
	int PATCH_VERT_RESOLUTION = TILE_RESOLUTION + 1;
	int CLIPMAP_RESOLUTION = TILE_RESOLUTION * 4 + 1;
	int CLIPMAP_VERT_RESOLUTION = CLIPMAP_RESOLUTION + 1;

	int n = 0;

	// Create a tile mesh
	// A tile is the main component of terrain panels
	// LOD0: 4 tiles are placed as a square in each center quadrant, for a total of 16 tiles
	// LOD1..N 3 tiles make up a corner, 4 corners uses 12 tiles
	{
		std::vector<VertexNormalTangent> vertices {};
		vertices.resize(PATCH_VERT_RESOLUTION * PATCH_VERT_RESOLUTION);
        std::vector<uint32_t> indices;
		indices.resize(TILE_RESOLUTION * TILE_RESOLUTION * 6);

		n = 0;

		for (int y = 0; y < PATCH_VERT_RESOLUTION; y++) {
			for (int x = 0; x < PATCH_VERT_RESOLUTION; x++) {
				vertices[n++].position = glm::vec3(x, 0.f, y);
			}
		}

		n = 0;

		for (int y = 0; y < TILE_RESOLUTION; y++) {
			for (int x = 0; x < TILE_RESOLUTION; x++) {
                indices[n++] = _patch_2d(x, y, PATCH_VERT_RESOLUTION);
                indices[n++] = _patch_2d(x, y + 1, PATCH_VERT_RESOLUTION);
                indices[n++] = _patch_2d(x + 1, y + 1, PATCH_VERT_RESOLUTION);

                indices[n++] = _patch_2d(x, y, PATCH_VERT_RESOLUTION);
                indices[n++] = _patch_2d(x + 1, y + 1, PATCH_VERT_RESOLUTION);
                indices[n++] = _patch_2d(x + 1, y, PATCH_VERT_RESOLUTION);
			}
		}

		tile_mesh = _create_mesh(std::move(vertices), std::move(indices), "tile");
	}

	// Create a filler mesh
	// These meshes are small strips that fill in the gaps between LOD1+,
	// but only on the camera X and Z axes, and not on LOD0.
	{
		std::vector<VertexNormalTangent> vertices;
		vertices.resize(PATCH_VERT_RESOLUTION * 8);
		std::vector<uint32_t> indices;
		indices.resize(TILE_RESOLUTION * 24);

		n = 0;
		int offset = TILE_RESOLUTION;

		for (int i = 0; i < PATCH_VERT_RESOLUTION; i++) {
			vertices[n].position = glm::vec3(offset + i + 1.f, 0.f, 0.f);
			n++;

			vertices[n].position = glm::vec3(offset + i + 1.f, 0.f, 1.f);
			n++;
		}

		for (int i = 0; i < PATCH_VERT_RESOLUTION; i++) {
			vertices[n].position = glm::vec3(1.f, 0.f, offset + i + 1.f);
			n++;

			vertices[n].position = glm::vec3(0.f, 0.f, offset + i + 1.f);
			n++;
		}

		for (int i = 0; i < PATCH_VERT_RESOLUTION; i++) {
			vertices[n].position = glm::vec3(-float(offset + i), 0.f, 1.f);
			n++;

			vertices[n].position = glm::vec3(-float(offset + i), 0.f, 0.f);
			n++;
		}

		for (int i = 0; i < PATCH_VERT_RESOLUTION; i++) {
			vertices[n].position = glm::vec3(0.f, 0.f, -float(offset + i));
			n++;

			vertices[n].position = glm::vec3(1.f, 0.f, -float(offset + i));
			n++;
		}

		n = 0;
		for (int i = 0; i < TILE_RESOLUTION * 4; i++) {
			int arm = i / TILE_RESOLUTION;

			int bl = (arm + i) * 2 + 0;
			int br = (arm + i) * 2 + 1;
			int tl = (arm + i) * 2 + 2;
			int tr = (arm + i) * 2 + 3;

            if (arm % 2 == 0) {
                indices[n++] = tr;
                indices[n++] = bl;
                indices[n++] = br;
                indices[n++] = tr;
                indices[n++] = tl;
                indices[n++] = bl;
            } else {
                indices[n++] = tl;
                indices[n++] = bl;
                indices[n++] = br;
                indices[n++] = tl;
                indices[n++] = br;
                indices[n++] = tr;
            }
		}

		filler_mesh = _create_mesh(std::move(vertices), std::move(indices), "filler");
	}

	// Create trim mesh
	// This mesh is a skinny L shape that fills in the gaps between
	// LOD meshes when they are moving at different speeds and have gaps
	{
		std::vector<VertexNormalTangent> vertices;
		vertices.resize((CLIPMAP_VERT_RESOLUTION * 2 + 1) * 2);
		std::vector<uint32_t> indices;
		indices.resize((CLIPMAP_VERT_RESOLUTION * 2 - 1) * 6);

		n = 0;
		glm::vec3 offset = glm::vec3(0.5f * float(CLIPMAP_VERT_RESOLUTION + 1), 0.f, 0.5f * float(CLIPMAP_VERT_RESOLUTION + 1));

		for (int i = 0; i < CLIPMAP_VERT_RESOLUTION + 1; i++) {
			vertices[n].position = glm::vec3(0.f, 0.f, CLIPMAP_VERT_RESOLUTION - i) - offset;
			n++;

			vertices[n].position = glm::vec3(1.f, 0.f, CLIPMAP_VERT_RESOLUTION - i) - offset;
			n++;
		}

		int start_of_horizontal = n;

		for (int i = 0; i < CLIPMAP_VERT_RESOLUTION; i++) {
			vertices[n].position = glm::vec3(i + 1.f, 0.f, 0.f) - offset;
			n++;

			vertices[n].position = glm::vec3(i + 1.f, 0.f, 1.f) - offset;
			n++;
		}

		n = 0;

		for (int i = 0; i < CLIPMAP_VERT_RESOLUTION; i++) {
            indices[n++] = (i + 1) * 2 + 0;
            indices[n++] = (i + 0) * 2 + 0;
            indices[n++] = (i + 0) * 2 + 1;

            indices[n++] = (i + 1) * 2 + 0;
            indices[n++] = (i + 0) * 2 + 1;
            indices[n++] = (i + 1) * 2 + 1;
		}

		for (int i = 0; i < CLIPMAP_VERT_RESOLUTION - 1; i++) {
            indices[n++] = start_of_horizontal + (i + 1) * 2 + 0;
            indices[n++] = start_of_horizontal + (i + 0) * 2 + 0;
            indices[n++] = start_of_horizontal + (i + 0) * 2 + 1;

            indices[n++] = start_of_horizontal + (i + 1) * 2 + 0;
            indices[n++] = start_of_horizontal + (i + 0) * 2 + 1;
            indices[n++] = start_of_horizontal + (i + 1) * 2 + 1;
		}

		trim_mesh = _create_mesh(std::move(vertices), std::move(indices), "trim");
	}

	// Create center cross mesh
	// This mesh is the small cross shape that fills in the gaps along the
	// X and Z axes between the center quadrants on LOD0.
	{
		std::vector<VertexNormalTangent> vertices;
		vertices.resize(PATCH_VERT_RESOLUTION * 8);
		std::vector<uint32_t> indices;
		indices.resize(TILE_RESOLUTION * 24 + 6);

		n = 0;

		for (int i = 0; i < PATCH_VERT_RESOLUTION * 2; i++) {
			vertices[n].position = glm::vec3(float(i - TILE_RESOLUTION), 0.f, 0.f);
			n++;

			vertices[n].position = glm::vec3(float(i - TILE_RESOLUTION), 0.f, 1.f);
			n++;
		}

		int start_of_vertical = n;

		for (int i = 0; i < PATCH_VERT_RESOLUTION * 2; i++) {
			vertices[n].position = glm::vec3(0.f, 0.f, float(i - TILE_RESOLUTION));
			n++;

			vertices[n].position = glm::vec3(1.f, 0.f, float(i - TILE_RESOLUTION));
			n++;
		}

		n = 0;

		for (int i = 0; i < TILE_RESOLUTION * 2 + 1; i++) {
			int bl = i * 2 + 0;
			int br = i * 2 + 1;
			int tl = i * 2 + 2;
			int tr = i * 2 + 3;

            indices[n++] = tr;
            indices[n++] = bl;
            indices[n++] = br;
            indices[n++] = tl;
            indices[n++] = bl;
            indices[n++] = tr;
		}

		for (int i = 0; i < TILE_RESOLUTION * 2 + 1; i++) {
			if (i == TILE_RESOLUTION) {
				continue;
			}

			int bl = i * 2 + 0;
			int br = i * 2 + 1;
			int tl = i * 2 + 2;
			int tr = i * 2 + 3;

            indices[n++] = start_of_vertical + bl;
            indices[n++] = start_of_vertical + tr;
            indices[n++] = start_of_vertical + br;
            indices[n++] = start_of_vertical + tl;
            indices[n++] = start_of_vertical + tr;
            indices[n++] = start_of_vertical + bl;
		}

		cross_mesh = _create_mesh(std::move(vertices), std::move(indices), "cross");
	}

	// Create seam mesh
	// This is a very thin mesh that is supposed to cover tiny gaps
	// between tiles and fillers when the vertices do not line up
	{
		std::vector<VertexNormalTangent> vertices;
		vertices.resize(CLIPMAP_VERT_RESOLUTION * 4);
		std::vector<uint32_t> indices;
		indices.resize(CLIPMAP_VERT_RESOLUTION * 6);

		n = 0;

		for (int i = 0; i < CLIPMAP_VERT_RESOLUTION; i++) {
			n = CLIPMAP_VERT_RESOLUTION * 0 + i;
			vertices[n].position = glm::vec3(i, 0.f, 0.f);

			n = CLIPMAP_VERT_RESOLUTION * 1 + i;
			vertices[n].position = glm::vec3(CLIPMAP_VERT_RESOLUTION, 0.f, i);

			n = CLIPMAP_VERT_RESOLUTION * 2 + i;
			vertices[n].position = glm::vec3(CLIPMAP_VERT_RESOLUTION - i, 0.f, CLIPMAP_VERT_RESOLUTION);

			n = CLIPMAP_VERT_RESOLUTION * 3 + i;
			vertices[n].position = glm::vec3(0.f, 0.f, CLIPMAP_VERT_RESOLUTION - i);
		}

		n = 0;

		for (int i = 0; i < CLIPMAP_VERT_RESOLUTION * 4; i += 2) {
            indices[n++] = i;     // изменено
            indices[n++] = i + 1; // изменено
            indices[n++] = i + 2; // изменено
        }

		indices[indices.size() - 1] = 0;

		seam_mesh = _create_mesh(std::move(vertices), std::move(indices), "seam");
	}

	return {
		tile_mesh,
		filler_mesh,
		trim_mesh,
		cross_mesh,
		seam_mesh
	};
}
