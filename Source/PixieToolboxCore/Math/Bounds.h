#pragma once
#include "MathBase.h"

struct Bounds2i {
	glm::ivec2 min = glm::ivec2(std::numeric_limits<int32_t>::max());
	glm::ivec2 max = glm::ivec2(std::numeric_limits<int32_t>::lowest());

	Bounds2i() = default;
	explicit Bounds2i(glm::ivec2 p);
	Bounds2i(glm::ivec2 p1, glm::ivec2 p2);

	glm::ivec2 Diagonal() const;
	glm::ivec2 Center() const;
	float Area() const;
	int32_t MaxDimension() const;
	void BoundingCircle(glm::vec2* center, float* radius) const;
	bool IsEmpty() const;
	bool IsDegenerate() const;

	glm::ivec2 operator[](int32_t i) const;
	glm::ivec2& operator[](int32_t i);
	bool operator==(const Bounds2i& b) const;
	bool operator!=(const Bounds2i& b) const;
};

struct Bounds2f {
	glm::vec2 min = glm::vec2(std::numeric_limits<float>::max());
	glm::vec2 max = glm::vec2(std::numeric_limits<float>::lowest());

	Bounds2f() = default;
	explicit Bounds2f(glm::vec2 p);
	Bounds2f(glm::vec2 p1, glm::vec2 p2);

	glm::vec2 Diagonal() const;
	glm::vec2 Center() const;
	float Area() const;
	int32_t MaxDimension() const;
	glm::vec2 Offset(glm::vec2 p) const;
	void BoundingCircle(glm::vec2* center, float* radius) const;
	bool IsEmpty() const;
	bool IsDegenerate() const;

	glm::vec2 operator[](int32_t i) const;
	glm::vec2& operator[](int32_t i);
	bool operator==(const Bounds2f& b) const;
	bool operator!=(const Bounds2f& b) const;
};

struct Bounds3i {
	glm::ivec3 min = glm::ivec3(std::numeric_limits<int32_t>::max());
	glm::ivec3 max = glm::ivec3(std::numeric_limits<int32_t>::lowest());

	Bounds3i() = default;
	explicit Bounds3i(glm::ivec3 p);
	Bounds3i(glm::ivec3 p1, glm::ivec3 p2);

	glm::ivec3 Corner(int32_t corner) const;
	glm::ivec3 Diagonal() const;
	glm::ivec3 Center() const;
	float Area() const;
	float Volume() const;
	int32_t MaxDimension() const;
	glm::ivec3 Offset(glm::ivec3 p) const;
	void BoundingSphere(glm::vec3* center, float* radius) const;
	bool IsEmpty() const;
	bool IsDegenerate() const;

	glm::ivec3 operator[](int32_t i) const;
	glm::ivec3& operator[](int32_t i);
	bool operator==(const Bounds3i& b) const;
	bool operator!=(const Bounds3i& b) const;
};

struct Bounds3f {
	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

	Bounds3f() = default;
	explicit Bounds3f(glm::vec3 p);
	Bounds3f(glm::vec3 p1, glm::vec3 p2);

	glm::vec3 Corner(int32_t corner) const;
	glm::vec3 Diagonal() const;
	glm::vec3 Center() const;
	float Area() const;
	float Volume() const;
	int32_t MaxDimension() const;
	glm::vec3 Offset(glm::vec3 p) const;
	void BoundingSphere(glm::vec3* center, float* radius) const;
	bool IsEmpty() const;
	bool IsDegenerate() const;

	glm::vec3 operator[](int32_t i) const;
	glm::vec3& operator[](int32_t i);
	bool operator==(const Bounds3f& b) const;
	bool operator!=(const Bounds3f& b) const;
};

Bounds2i Union(const Bounds2i& b1, const Bounds2i& b2);
Bounds2f Union(const Bounds2f& b1, const Bounds2f& b2);
Bounds3i Union(const Bounds3i& b1, const Bounds3i& b2);
Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2);
Bounds3f Union(const Bounds3f& b, glm::vec3 p);
bool Inside(glm::vec3 p, const Bounds3f& b);
