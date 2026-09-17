#include "MeshGenerator.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include <glm/gtc/constants.hpp>

namespace PixieToolbox {

namespace {

inline Vertex MakeVertex(glm::vec3 pos, glm::vec3 nrm = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2 uv = glm::vec2(0.0f)) {
	Vertex v;
	v.position = pos;
	v.normal = nrm;
	v.uv = uv;
	return v;
}

} // namespace

Mesh MeshGenerator::Quad(glm::vec2 min, glm::vec2 max) {
	Mesh mesh;
	mesh.vertexes = {
		MakeVertex({ min.x, min.y, 0.0f }, { 0, 0, 1 }, { 0, 0 }),
		MakeVertex({ min.x, max.y, 0.0f }, { 0, 0, 1 }, { 0, 1 }),
		MakeVertex({ max.x, max.y, 0.0f }, { 0, 0, 1 }, { 1, 1 }),
		MakeVertex({ max.x, min.y, 0.0f }, { 0, 0, 1 }, { 1, 0 }),
	};
	mesh.indexes = { 0, 1, 2, 0, 2, 3 };
	return mesh;
}

Mesh MeshGenerator::Grid(glm::vec2 size, glm::ivec2 resolution) {
	Mesh mesh;

	resolution.x = std::max(resolution.x, 2);
	resolution.y = std::max(resolution.y, 2);

	const float invX = 1.0f / static_cast<float>(resolution.x - 1);
	const float invY = 1.0f / static_cast<float>(resolution.y - 1);

	const glm::vec3 scale = { size.x * invX, 1.0f, size.y * invY };

	const float invW = 1.0f / static_cast<float>(resolution.x - 1);
	const float invH = 1.0f / static_cast<float>(resolution.y - 1);

	const uint32_t vx = static_cast<uint32_t>(resolution.x);
	const uint32_t vy = static_cast<uint32_t>(resolution.y);

	mesh.vertexes.reserve(vx * vy);
	for (uint32_t y = 0; y < vy; ++y) {
		for (uint32_t x = 0; x < vx; ++x) {
			const glm::vec3 pos = glm::vec3(static_cast<float>(x), 0.0f, static_cast<float>(y)) * scale;
			const glm::vec2 uv = glm::vec2(static_cast<float>(x) * invW, static_cast<float>(y) * invH);
			mesh.vertexes.push_back(MakeVertex(pos, { 0, 1, 0 }, uv));
		}
	}

	mesh.indexes.reserve(static_cast<size_t>(vx - 1) * (vy - 1) * 6);
	for (uint32_t y = 0; y + 1 < vy; ++y) {
		for (uint32_t x = 0; x + 1 < vx; ++x) {
			const uint32_t i0 = y * vx + x;
			const uint32_t i1 = i0 + 1;
			const uint32_t i2 = i0 + vx + 1;
			const uint32_t i3 = i0 + vx;

			mesh.indexes.push_back(i0);
			mesh.indexes.push_back(i1);
			mesh.indexes.push_back(i2);

			mesh.indexes.push_back(i0);
			mesh.indexes.push_back(i2);
			mesh.indexes.push_back(i3);
		}
	}

	return mesh;
}

Mesh MeshGenerator::Cube(glm::vec3 size) {
	const glm::vec3 hs = size * 0.5f;

	Mesh mesh;
	mesh.vertexes.reserve(36);

	const glm::vec3 n[6] = {
		{ 0, 0, -1 }, { 0, 0, 1 }, { -1, 0, 0 }, { 1, 0, 0 }, { 0, -1, 0 }, { 0, 1, 0 },
	};

	auto pushFace = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 nrm) {
		mesh.vertexes.push_back(MakeVertex(a, nrm, { 0, 0 }));
		mesh.vertexes.push_back(MakeVertex(b, nrm, { 1, 0 }));
		mesh.vertexes.push_back(MakeVertex(c, nrm, { 1, 1 }));

		mesh.vertexes.push_back(MakeVertex(a, nrm, { 0, 0 }));
		mesh.vertexes.push_back(MakeVertex(c, nrm, { 1, 1 }));
		mesh.vertexes.push_back(MakeVertex(d, nrm, { 0, 1 }));
	};

	pushFace({ -hs.x, -hs.y, -hs.z }, { hs.x, -hs.y, -hs.z }, { hs.x, hs.y, -hs.z }, { -hs.x, hs.y, -hs.z }, n[0]);
	pushFace({ -hs.x, -hs.y, hs.z }, { hs.x, -hs.y, hs.z }, { hs.x, hs.y, hs.z }, { -hs.x, hs.y, hs.z }, n[1]);
	pushFace({ -hs.x, hs.y, -hs.z }, { -hs.x, hs.y, hs.z }, { -hs.x, -hs.y, hs.z }, { -hs.x, -hs.y, -hs.z }, n[2]);
	pushFace({ hs.x, hs.y, hs.z }, { hs.x, hs.y, -hs.z }, { hs.x, -hs.y, -hs.z }, { hs.x, -hs.y, hs.z }, n[3]);
	pushFace({ -hs.x, -hs.y, -hs.z }, { hs.x, -hs.y, -hs.z }, { hs.x, -hs.y, hs.z }, { -hs.x, -hs.y, hs.z }, n[4]);
	pushFace({ -hs.x, hs.y, -hs.z }, { -hs.x, hs.y, hs.z }, { hs.x, hs.y, hs.z }, { hs.x, hs.y, -hs.z }, n[5]);

	mesh.indexes.resize(mesh.vertexes.size());
	for (uint32_t i = 0; i < mesh.indexes.size(); ++i)
		mesh.indexes[i] = i;

	return mesh;
}

Mesh MeshGenerator::SphereFromOctahedron(float radius, uint32_t subdivisions) {
	subdivisions = std::min(subdivisions, 7u);

	std::vector<glm::vec3> positions = {
		{ 0, 1, 0 }, { 1, 0, 0 }, { 0, 0, -1 }, { -1, 0, 0 }, { 0, 0, 1 }, { 0, -1, 0 },
	};

	std::vector<uint32_t> indices = {
		0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1, 5, 1, 2, 5, 2, 3, 5, 3, 4, 5, 4, 1,
	};

	std::unordered_map<uint64_t, uint32_t> cache;

	constexpr unsigned kMaxSubdivisions = 15;
	if (subdivisions > kMaxSubdivisions) {
		subdivisions = kMaxSubdivisions;
	}
	const size_t newItems = size_t{ 1 } << (subdivisions * 2);
	const size_t total = positions.size() * newItems;

	cache.reserve(total);

	auto midpoint = [&](uint32_t a, uint32_t b) -> uint32_t {
		const uint32_t lo = std::min(a, b);
		const uint32_t hi = std::max(a, b);
		const uint64_t key = (static_cast<uint64_t>(lo) << 32) | hi;

		auto it = cache.find(key);
		if (it != cache.end())
			return it->second;

		const glm::vec3 m = glm::normalize((positions[a] + positions[b]) * 0.5f);
		const uint32_t idx = static_cast<uint32_t>(positions.size());
		positions.push_back(m);
		cache.emplace(key, idx);
		return idx;
	};

	std::vector<uint32_t> next;
	for (uint32_t s = 0; s < subdivisions; ++s) {
		next.clear();
		next.reserve(indices.size() * 4);

		for (size_t i = 0; i < indices.size(); i += 3) {
			const uint32_t a = indices[i];
			const uint32_t b = indices[i + 1];
			const uint32_t c = indices[i + 2];

			const uint32_t ab = midpoint(a, b);
			const uint32_t bc = midpoint(b, c);
			const uint32_t ca = midpoint(c, a);

			next.push_back(a);
			next.push_back(ab);
			next.push_back(ca);
			next.push_back(ab);
			next.push_back(b);
			next.push_back(bc);
			next.push_back(ca);
			next.push_back(bc);
			next.push_back(c);
			next.push_back(ab);
			next.push_back(bc);
			next.push_back(ca);
		}

		indices.swap(next);
	}

	Mesh mesh;
	mesh.vertexes.reserve(positions.size());
	mesh.indexes = std::move(indices);

	constexpr float kTwoPi = glm::two_pi<float>();
	constexpr float kPi = glm::pi<float>();

	for (const glm::vec3& p : positions) {
		const glm::vec3 n = glm::normalize(p);
		Vertex v;
		v.position = n * radius;
		v.normal = n;
		v.uv = glm::vec2(0.5f + std::atan2(n.x, n.z) / kTwoPi, 0.5f - std::asin(glm::clamp(n.y, -1.0f, 1.0f)) / kPi);
		mesh.vertexes.push_back(v);
	}

	return mesh;
}

Mesh MeshGenerator::UVSphere(float radius, uint32_t rings, uint32_t segments) {
	rings = std::max(rings, 3u);
	segments = std::max(segments, 3u);

	Mesh mesh;
	const uint32_t vx = segments + 1;
	const uint32_t vy = rings + 1;

	mesh.vertexes.reserve(vx * vy);
	for (uint32_t y = 0; y <= rings; ++y) {
		const float v = static_cast<float>(y) / static_cast<float>(rings);
		const float phi = v * glm::pi<float>();

		for (uint32_t x = 0; x <= segments; ++x) {
			const float u = static_cast<float>(x) / static_cast<float>(segments);
			const float theta = u * glm::two_pi<float>(); // 0..2π

			const glm::vec3 n = {
				std::sin(phi) * std::cos(theta),
				std::cos(phi),
				std::sin(phi) * std::sin(theta),
			};

			Vertex vert;
			vert.position = n * radius;
			vert.normal = n;
			vert.uv = { u, 1.0f - v };
			mesh.vertexes.push_back(vert);
		}
	}

	mesh.indexes.reserve(static_cast<size_t>(rings) * segments * 6);
	for (uint32_t y = 0; y < rings; ++y) {
		for (uint32_t x = 0; x < segments; ++x) {
			const uint32_t i0 = y * vx + x;
			const uint32_t i1 = i0 + 1;
			const uint32_t i2 = i0 + vx + 1;
			const uint32_t i3 = i0 + vx;

			mesh.indexes.push_back(i0);
			mesh.indexes.push_back(i2);
			mesh.indexes.push_back(i1);

			mesh.indexes.push_back(i0);
			mesh.indexes.push_back(i3);
			mesh.indexes.push_back(i2);
		}
	}

	return mesh;
}

Mesh MeshGenerator::ScreenTriangle() {
	Mesh mesh;
	mesh.vertexes = {
		MakeVertex({ -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }),
		MakeVertex({ 3.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 2.0f, 0.0f }),
		MakeVertex({ -1.0f, 3.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 2.0f }),
	};
	mesh.indexes = { 0, 1, 2 };
	return mesh;
}

Mesh MeshGenerator::ScreenTriangleNormalizedUV() {
	Mesh mesh;
	mesh.vertexes = {
		MakeVertex({ -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }),
		MakeVertex({ 3.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }),
		MakeVertex({ -1.0f, 3.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }),
	};
	mesh.indexes = { 0, 1, 2 };
	return mesh;
}

} // namespace PixieToolbox
