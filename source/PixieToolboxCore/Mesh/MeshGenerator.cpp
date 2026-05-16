#include "MeshGenerator.h"

#include <numbers>

Mesh* MeshGenerator::Quad(glm::vec2 min, glm::vec2 max) {
	Mesh* mesh = new Mesh();
	mesh->m_positions = {
		glm::vec3(min.x, min.y, 0),
		glm::vec3(min.x, max.y, 0),
		glm::vec3(max.x, max.y, 0),
		glm::vec3(max.x, min.y, 0),
	};
	mesh->m_indices = {
		0, 1, 2,
		0, 2, 3
	};
	return mesh;
}

Mesh* MeshGenerator::Grid(glm::vec2 size, glm::ivec2 resolution) {
	Mesh* mesh = new Mesh();

	glm::vec3 scale = glm::vec3(size.x, 1.0f, size.y) / glm::vec3(resolution.x, 1.0f, resolution.y);

	mesh->m_positions.reserve(resolution.x * resolution.y);
	mesh->m_texCoords.reserve(resolution.x * resolution.y);
	for (int32_t y = 0; y < resolution.y; y++) {
		for (int32_t x = 0; x < resolution.x; x++) {
			mesh->m_positions.push_back(glm::vec3(x, 0, y) * scale);
			mesh->m_texCoords.push_back(glm::vec2(x, y) / glm::vec2(resolution - glm::ivec2(1, 1)));
		}
	}

	mesh->m_indices.reserve(resolution.x * resolution.y * 6);
	for (int32_t y = 0; y < resolution.y - 1; y++) {
		for (int32_t x = 0; x < resolution.x - 1; x++) {
			mesh->m_indices.push_back(y * resolution.x + x);
			mesh->m_indices.push_back(y * resolution.x + x + 1);
			mesh->m_indices.push_back((y + 1) * resolution.x + x + 1);

			mesh->m_indices.push_back(y * resolution.x + x);
			mesh->m_indices.push_back((y + 1) * resolution.x + x + 1);
			mesh->m_indices.push_back((y + 1) * resolution.x + x);
		}
	}
	
	return mesh;
}

Mesh* MeshGenerator::Cube(glm::vec3 size) {
	glm::vec3 hs = size / 2.0f;

	Mesh* mesh = new Mesh();

	mesh->m_positions = {
		glm::vec3(-hs.x, -hs.y, -hs.z),
		glm::vec3(hs.x, -hs.y, -hs.z),
		glm::vec3(hs.x,  hs.y, -hs.z),
		glm::vec3(hs.x,  hs.y, -hs.z),
		glm::vec3(-hs.x,  hs.y, -hs.z),
		glm::vec3(-hs.x, -hs.y, -hs.z),

		glm::vec3(-hs.x, -hs.y,  hs.z),
		glm::vec3(hs.x, -hs.y,  hs.z),
		glm::vec3(hs.x,  hs.y,  hs.z),
		glm::vec3(hs.x,  hs.y,  hs.z),
		glm::vec3(-hs.x,  hs.y,  hs.z),
		glm::vec3(-hs.x, -hs.y,  hs.z),

		glm::vec3(-hs.x,  hs.y,  hs.z),
		glm::vec3(-hs.x,  hs.y, -hs.z),
		glm::vec3(-hs.x, -hs.y, -hs.z),
		glm::vec3(-hs.x, -hs.y, -hs.z),
		glm::vec3(-hs.x, -hs.y,  hs.z),
		glm::vec3(-hs.x,  hs.y,  hs.z),

		glm::vec3(hs.x,  hs.y,  hs.z),
		glm::vec3(hs.x,  hs.y, -hs.z),
		glm::vec3(hs.x, -hs.y, -hs.z),
		glm::vec3(hs.x, -hs.y, -hs.z),
		glm::vec3(hs.x, -hs.y,  hs.z),
		glm::vec3(hs.x,  hs.y,  hs.z),

		glm::vec3(-hs.x, -hs.y, -hs.z),
		glm::vec3(hs.x, -hs.y, -hs.z),
		glm::vec3(hs.x, -hs.y,  hs.z),
		glm::vec3(hs.x, -hs.y,  hs.z),
		glm::vec3(-hs.x, -hs.y,  hs.z),
		glm::vec3(-hs.x, -hs.y, -hs.z),

		glm::vec3(-hs.x,  hs.y, -hs.z),
		glm::vec3(hs.x,  hs.y, -hs.z),
		glm::vec3(hs.x,  hs.y,  hs.z),
		glm::vec3(hs.x,  hs.y,  hs.z),
		glm::vec3(-hs.x,  hs.y,  hs.z),
		glm::vec3(-hs.x,  hs.y, -hs.z)
	};

	mesh->m_normals = {
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),

		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),

		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),

		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),

		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0)
	};

	mesh->m_texCoords = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
							 
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	};

	mesh->m_indices = {
		 0,  1,  2,  3,  4,  5,
		 6,  7,  8,  9, 10, 11,
		12, 13, 14, 15, 16, 17,
		18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35
	};

	return mesh;
}

Mesh* MeshGenerator::SphereFromOctahedron(float radius, uint32_t subdivisions) {
	if (subdivisions > 10) subdivisions = 10;

	std::vector<glm::vec3> positions = {
		glm::vec3(0, 1, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(0, 0, -1),
		glm::vec3(-1, 0, 0),
		glm::vec3(0, 0, 1),
		glm::vec3(0, -1, 0)
	};

	std::vector<int32_t> indices = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
		5, 1, 2,
		5, 2, 3,
		5, 3, 4,
		5, 4, 1
	};

	std::vector<int32_t> newIndices;
	for (uint32_t subdivision = 0; subdivision < subdivisions; subdivision++) {
		for (size_t i = 0; i < indices.size(); i += 3) {
			positions.push_back({ (positions[indices[i + 0]] + positions[indices[i + 1]]) / 2.0f });
			positions.push_back({ (positions[indices[i + 1]] + positions[indices[i + 2]]) / 2.0f });
			positions.push_back({ (positions[indices[i + 2]] + positions[indices[i + 0]]) / 2.0f });

			newIndices.push_back(indices[i + 0]);
			newIndices.push_back((int32_t)positions.size() - 3);
			newIndices.push_back((int32_t)positions.size() - 1);

			newIndices.push_back((int32_t)positions.size() - 3);
			newIndices.push_back(indices[i + 1]);
			newIndices.push_back((int32_t)positions.size() - 2);

			newIndices.push_back((int32_t)positions.size() - 3);
			newIndices.push_back((int32_t)positions.size() - 2);
			newIndices.push_back((int32_t)positions.size() - 1);

			newIndices.push_back((int32_t)positions.size() - 1);
			newIndices.push_back((int32_t)positions.size() - 2);
			newIndices.push_back(indices[i + 2]);
		}
		indices = newIndices;
		newIndices.clear();
	}

	Mesh* mesh = new Mesh();

	for (size_t i = 0; i < positions.size(); i++) {
		glm::vec3 n = glm::normalize(positions[i]);
		mesh->m_positions.push_back(n * radius);
		mesh->m_normals.push_back(n);
		mesh->m_texCoords.push_back(glm::vec2(0.5f + (float)std::atan2(n.x, n.z) / (std::numbers::pi * 2.0f), n.y));
	}

	mesh->m_indices = indices;

	return mesh;
}
