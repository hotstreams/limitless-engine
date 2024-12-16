#include <limitless/core/indexed_stream.hpp>
#include "geoclipmap.h"

using namespace Limitless;

std::shared_ptr<AbstractMesh> GeoClipMap::_create_mesh(
        std::vector<VertexNormalTangent>&& p_vertices,
        std::vector<uint32_t>&& p_indices,
        std::string name
) {
//    static int i = 0;
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

// Generate clipmap meshes originally by Mike J Savage
std::vector<std::shared_ptr<AbstractMesh>> GeoClipMap::generate(const int p_size, const int p_levels) {
//	LOG(DEBUG, "Generating meshes of size: ", p_size, " levels: ", p_levels);

	// TODO bit of a mess here. someone care to clean up?
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
	int NUM_CLIPMAP_LEVELS = p_levels;

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

//		aabb = AABB(glm::vec3(0.f, 0.f, 0.f), glm::vec3(PATCH_VERT_RESOLUTION, 0.1f, PATCH_VERT_RESOLUTION));
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
//			aabb.expand_to(vertices[n]);
			n++;

			vertices[n].position = glm::vec3(offset + i + 1.f, 0.f, 1.f);
//			aabb.expand_to(vertices[n]);
			n++;
		}

		for (int i = 0; i < PATCH_VERT_RESOLUTION; i++) {
			vertices[n].position = glm::vec3(1.f, 0.f, offset + i + 1.f);
//			aabb.expand_to(vertices[n]);
			n++;

			vertices[n].position = glm::vec3(0.f, 0.f, offset + i + 1.f);
//			aabb.expand_to(vertices[n]);
			n++;
		}

		for (int i = 0; i < PATCH_VERT_RESOLUTION; i++) {
			vertices[n].position = glm::vec3(-float(offset + i), 0.f, 1.f);
//			aabb.expand_to(vertices[n]);
			n++;

			vertices[n].position = glm::vec3(-float(offset + i), 0.f, 0.f);
//			aabb.expand_to(vertices[n]);
			n++;
		}

		for (int i = 0; i < PATCH_VERT_RESOLUTION; i++) {
			vertices[n].position = glm::vec3(0.f, 0.f, -float(offset + i));
//			aabb.expand_to(vertices[n]);
			n++;

			vertices[n].position = glm::vec3(1.f, 0.f, -float(offset + i));
//			aabb.expand_to(vertices[n]);
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
                indices[n++] = tr; // изменено
                indices[n++] = bl; // изменено
                indices[n++] = br; // изменено
                indices[n++] = tr; // изменено
                indices[n++] = tl; // изменено
                indices[n++] = bl; // изменено
            } else {
                indices[n++] = tl; // изменено
                indices[n++] = bl; // изменено
                indices[n++] = br; // изменено
                indices[n++] = tl; // изменено
                indices[n++] = br; // изменено
                indices[n++] = tr; // изменено
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
//			aabb.expand_to(vertices[n]);
			n++;

			vertices[n].position = glm::vec3(1.f, 0.f, CLIPMAP_VERT_RESOLUTION - i) - offset;
//			aabb.expand_to(vertices[n]);
			n++;
		}

		int start_of_horizontal = n;

		for (int i = 0; i < CLIPMAP_VERT_RESOLUTION; i++) {
			vertices[n].position = glm::vec3(i + 1.f, 0.f, 0.f) - offset;
//			aabb.expand_to(vertices[n]);
			n++;

			vertices[n].position = glm::vec3(i + 1.f, 0.f, 1.f) - offset;
//			aabb.expand_to(vertices[n]);
			n++;
		}

		n = 0;

		for (int i = 0; i < CLIPMAP_VERT_RESOLUTION; i++) {
            indices[n++] = (i + 1) * 2 + 0; // изменено
            indices[n++] = (i + 0) * 2 + 0; // изменено
            indices[n++] = (i + 0) * 2 + 1; // изменено

            indices[n++] = (i + 1) * 2 + 0; // изменено
            indices[n++] = (i + 0) * 2 + 1; // изменено
            indices[n++] = (i + 1) * 2 + 1; // изменено
		}

		for (int i = 0; i < CLIPMAP_VERT_RESOLUTION - 1; i++) {
            indices[n++] = start_of_horizontal + (i + 1) * 2 + 0; // изменено
            indices[n++] = start_of_horizontal + (i + 0) * 2 + 0; // изменено
            indices[n++] = start_of_horizontal + (i + 0) * 2 + 1; // изменено

            indices[n++] = start_of_horizontal + (i + 1) * 2 + 0; // изменено
            indices[n++] = start_of_horizontal + (i + 0) * 2 + 1; // изменено
            indices[n++] = start_of_horizontal + (i + 1) * 2 + 1; // изменено
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
//			aabb.expand_to(vertices[n]);
			n++;

			vertices[n].position = glm::vec3(float(i - TILE_RESOLUTION), 0.f, 1.f);
//			aabb.expand_to(vertices[n]);
			n++;
		}

		int start_of_vertical = n;

		for (int i = 0; i < PATCH_VERT_RESOLUTION * 2; i++) {
			vertices[n].position = glm::vec3(0.f, 0.f, float(i - TILE_RESOLUTION));
//			aabb.expand_to(vertices[n]);
			n++;

			vertices[n].position = glm::vec3(1.f, 0.f, float(i - TILE_RESOLUTION));
//			aabb.expand_to(vertices[n]);
			n++;
		}

		n = 0;

		for (int i = 0; i < TILE_RESOLUTION * 2 + 1; i++) {
			int bl = i * 2 + 0;
			int br = i * 2 + 1;
			int tl = i * 2 + 2;
			int tr = i * 2 + 3;

            indices[n++] = tr; // изменено
            indices[n++] = bl; // изменено
            indices[n++] = br; // изменено
            indices[n++] = tl; // изменено
            indices[n++] = bl; // изменено
            indices[n++] = tr; // изменено
		}

		for (int i = 0; i < TILE_RESOLUTION * 2 + 1; i++) {
			if (i == TILE_RESOLUTION) {
				continue;
			}

			int bl = i * 2 + 0;
			int br = i * 2 + 1;
			int tl = i * 2 + 2;
			int tr = i * 2 + 3;

            indices[n++] = start_of_vertical + bl; // изменено
            indices[n++] = start_of_vertical + tr; // изменено
            indices[n++] = start_of_vertical + br; // изменено
            indices[n++] = start_of_vertical + tl; // изменено
            indices[n++] = start_of_vertical + tr; // изменено
            indices[n++] = start_of_vertical + bl; // изменено
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
//			aabb.expand_to(vertices[n]);

			n = CLIPMAP_VERT_RESOLUTION * 1 + i;
			vertices[n].position = glm::vec3(CLIPMAP_VERT_RESOLUTION, 0.f, i);
//			aabb.expand_to(vertices[n]);

			n = CLIPMAP_VERT_RESOLUTION * 2 + i;
			vertices[n].position = glm::vec3(CLIPMAP_VERT_RESOLUTION - i, 0.f, CLIPMAP_VERT_RESOLUTION);
//			aabb.expand_to(vertices[n]);

			n = CLIPMAP_VERT_RESOLUTION * 3 + i;
			vertices[n].position = glm::vec3(0.f, 0.f, CLIPMAP_VERT_RESOLUTION - i);
//			aabb.expand_to(vertices[n]);
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

//	// skirt mesh
//    {
//		float scale = float(1 << (NUM_CLIPMAP_LEVELS - 1));
//		float fbase = float(TILE_RESOLUTION << NUM_CLIPMAP_LEVELS);
//		glm::vec2 base = -glm::vec2(fbase, fbase);
//
//        std::vector<VertexNormalTangent> vertices;
//
//		glm::vec2 clipmap_tl = base;
//		glm::vec2 clipmap_br = clipmap_tl + (glm::vec2(CLIPMAP_RESOLUTION, CLIPMAP_RESOLUTION) * scale);
//
//		float big = 10000000.0;
//
//        vertices.emplace_back(VertexNormalTangent{glm::vec3(-1, 0, -1) * big});
//        vertices.emplace_back(VertexNormalTangent{glm::vec3(+1, 0, -1) * big});
//        vertices.emplace_back(VertexNormalTangent{glm::vec3(-1, 0, +1) * big});
//        vertices.emplace_back(VertexNormalTangent{glm::vec3(+1, 0, +1) * big});
//        vertices.emplace_back(VertexNormalTangent{glm::vec3(clipmap_tl.x, 0, clipmap_tl.y)});
//        vertices.emplace_back(VertexNormalTangent{glm::vec3(clipmap_br.x, 0, clipmap_tl.y)});
//        vertices.emplace_back(VertexNormalTangent{glm::vec3(clipmap_tl.x, 0, clipmap_br.y)});
//        vertices.emplace_back(VertexNormalTangent{glm::vec3(clipmap_br.x, 0, clipmap_br.y)});
//
//        std::vector<uint32_t> indices{
//                0, 1, 4, 4, 1, 5,
//                1, 3, 5, 5, 3, 7,
//                3, 2, 7, 7, 2, 6,
//                4, 6, 0, 0, 6, 2
//        };
//
//        skirt_mesh = _create_mesh(std::move(vertices), std::move(indices), "skirt");
//    }

	return {
		tile_mesh,
		filler_mesh,
		trim_mesh,
		cross_mesh,
		seam_mesh,
//        skirt_mesh
	};
}
