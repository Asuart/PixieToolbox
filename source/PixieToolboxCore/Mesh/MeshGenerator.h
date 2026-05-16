#pragma once
#include "Mesh.h"

class MeshGenerator {
public:
	static Mesh* Quad(glm::vec2 min, glm::vec2 max);
	static Mesh* Grid(glm::vec2 size, glm::ivec2 resolution);
	static Mesh* Cube(glm::vec3 size = glm::vec3(1.0f));
	static Mesh* SphereFromOctahedron(float radius, uint32_t subdivisions);
};
