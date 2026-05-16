#include "Bounds.h"

Bounds2i::Bounds2i(glm::ivec2 p) :
	min(p), max(p) {
}

Bounds2i::Bounds2i(glm::ivec2 p1, glm::ivec2 p2) :
	min(glm::min(p1, p2)), max(glm::max(p1, p2)) {
}

glm::ivec2 Bounds2i::Diagonal() const {
	return max - min;
}

glm::ivec2 Bounds2i::Center() const {
	return min + Diagonal() / 2;
}

float Bounds2i::Area() const {
	glm::ivec2 diagonal = Diagonal();
	return (float)diagonal.x * diagonal.y;
}

int32_t Bounds2i::MaxDimension() const {
	glm::ivec2 diagonal = Diagonal();
	return diagonal.x >= diagonal.y ? 0 : 1;
}

void Bounds2i::BoundingCircle(glm::vec2* center, float* radius) const {
	*center = Center();
	*radius = glm::length(glm::vec2(Diagonal()) / 2.0f);
}

bool Bounds2i::IsEmpty() const {
	return min.x >= max.x || min.y >= max.y;
}

bool Bounds2i::IsDegenerate() const {
	return min.x > max.x || min.y > max.y;
}

glm::ivec2 Bounds2i::operator[](int32_t i) const {
	return (i == 0) ? min : max;
}

glm::ivec2& Bounds2i::operator[](int32_t i) {
	return (i == 0) ? min : max;
}

bool Bounds2i::operator==(const Bounds2i& b) const {
	return min == b.min && max == b.max;
}

bool Bounds2i::operator!=(const Bounds2i& b) const {
	return min != b.min || max != b.max;
}

Bounds2f::Bounds2f(glm::vec2 p) :
	min(p), max(p) {
}

Bounds2f::Bounds2f(glm::vec2 p1, glm::vec2 p2) :
	min(glm::min(p1, p2)), max(glm::max(p1, p2)) {
}

glm::vec2 Bounds2f::Diagonal() const {
	return max - min;
}

glm::vec2 Bounds2f::Center() const {
	return min + Diagonal() / 2.0f;
}

float Bounds2f::Area() const {
	glm::vec2 diagonal = Diagonal();
	return diagonal.x * diagonal.y;
}

int32_t Bounds2f::MaxDimension() const {
	glm::vec2 diagonal = Diagonal();
	return diagonal.x >= diagonal.y ? 0 : 1;
}

void Bounds2f::BoundingCircle(glm::vec2* center, float* radius) const {
	*center = Center();
	*radius = glm::length(Diagonal() / 2.0f);
}

bool Bounds2f::IsEmpty() const {
	return min.x >= max.x || min.y >= max.y;
}

bool Bounds2f::IsDegenerate() const {
	return min.x > max.x || min.y > max.y;
}

glm::vec2 Bounds2f::operator[](int32_t i) const {
	return (i == 0) ? min : max;
}

glm::vec2& Bounds2f::operator[](int32_t i) {
	return (i == 0) ? min : max;
}

bool Bounds2f::operator==(const Bounds2f& b) const {
	return min == b.min && max == b.max;
}

bool Bounds2f::operator!=(const Bounds2f& b) const {
	return min != b.min || max != b.max;
}

Bounds3i::Bounds3i(glm::ivec3 p) :
	min(p), max(p) {
}

Bounds3i::Bounds3i(glm::ivec3 p1, glm::ivec3 p2) :
	min(glm::min(p1, p2)), max(glm::max(p1, p2)) {
}

glm::ivec3 Bounds3i::Corner(int32_t corner) const {
	return glm::ivec3((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y, (*this)[(corner & 4) ? 1 : 0].z);
}

glm::ivec3 Bounds3i::Diagonal() const {
	return max - min;
}

glm::ivec3 Bounds3i::Center() const {
	return min + Diagonal() / 2;
}

float Bounds3i::Area() const {
	glm::ivec3 d = Diagonal();
	return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

float Bounds3i::Volume() const {
	glm::ivec3 d = Diagonal();
	return (float)d.x * d.y * d.z;
}

int32_t Bounds3i::MaxDimension() const {
	glm::ivec3 d = Diagonal();
	return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z ? 1 : 2);
}

glm::ivec3 Bounds3i::Offset(glm::ivec3 p) const {
	glm::ivec3 o = p - min;
	if (max.x > min.x) {
		o.x /= max.x - min.x;
	}
	if (max.y > min.y) {
		o.y /= max.y - min.y;
	}
	if (max.z > min.z) {
		o.z /= max.z - min.z;
	}
	return o;
}

void Bounds3i::BoundingSphere(glm::vec3* center, float* radius) const {
	*center = (glm::vec3)(min + max) / 2.0f;
	*radius = glm::length(((glm::vec3)min + (glm::vec3)max) / 2.0f);
}

bool Bounds3i::IsEmpty() const {
	return min.x >= max.x || min.y >= max.y || min.z >= max.z;
}

bool Bounds3i::IsDegenerate() const {
	return min.x > max.x || min.y > max.y || min.z > max.z;
}

bool Bounds3i::operator==(const Bounds3i& b) const {
	return b.min == min && b.max == max;
}

bool Bounds3i::operator!=(const Bounds3i& b) const {
	return b.min != min || b.max != max;
}

glm::ivec3 Bounds3i::operator[](int32_t i) const {
	return (i == 0) ? min : max;
}

glm::ivec3& Bounds3i::operator[](int32_t i) {
	return (i == 0) ? min : max;
}

Bounds3f::Bounds3f(glm::vec3 p) :
	min(p), max(p) {
}

Bounds3f::Bounds3f(glm::vec3 p1, glm::vec3 p2) :
	min(glm::min(p1, p2)), max(glm::max(p1, p2)) {
}

glm::vec3 Bounds3f::Corner(int32_t corner) const {
	return glm::vec3((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y, (*this)[(corner & 4) ? 1 : 0].z);
}

glm::vec3 Bounds3f::Diagonal() const {
	return max - min;
}

glm::vec3 Bounds3f::Center() const {
	return min + Diagonal() * 0.5f;
}

float Bounds3f::Area() const {
	glm::vec3 d = Diagonal();
	return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

float Bounds3f::Volume() const {
	glm::vec3 d = Diagonal();
	return d.x * d.y * d.z;
}

int32_t Bounds3f::MaxDimension() const {
	glm::vec3 d = Diagonal();
	return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z ? 1 : 2);
}

glm::vec3 Bounds3f::Offset(glm::vec3 p) const {
	glm::vec3 o = p - min;
	if (max.x > min.x) {
		o.x /= max.x - min.x;
	}
	if (max.y > min.y) {
		o.y /= max.y - min.y;
	}
	if (max.z > min.z) {
		o.z /= max.z - min.z;
	}
	return o;
}

void Bounds3f::BoundingSphere(glm::vec3* center, float* radius) const {
	*center = (min + max) / 2.0f;
	*radius = Inside(*center, *this) ? glm::distance(*center, max) : 0;
}

bool Bounds3f::IsEmpty() const {
	return min.x >= max.x || min.y >= max.y || min.z >= max.z;
}

bool Bounds3f::IsDegenerate() const {
	return min.x > max.x || min.y > max.y || min.z > max.z;
}

bool Bounds3f::operator==(const Bounds3f& b) const {
	return b.min == min && b.max == max;
}

bool Bounds3f::operator!=(const Bounds3f& b) const {
	return b.min != min || b.max != max;
}

glm::vec3 Bounds3f::operator[](int32_t i) const {
	return (i == 0) ? min : max;
}

glm::vec3& Bounds3f::operator[](int32_t i) {
	return (i == 0) ? min : max;
}

Bounds2f Union(const Bounds2f& b1, const Bounds2f& b2) {
	Bounds2f ret;
	ret.min = glm::min(b1.min, b2.min);
	ret.max = glm::max(b1.max, b2.max);
	return ret;
}

Bounds2i Union(const Bounds2i& b1, const Bounds2i& b2) {
	Bounds2i ret;
	ret.min = glm::min(b1.min, b2.min);
	ret.max = glm::max(b1.max, b2.max);
	return ret;
}

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2) {
	Bounds3f ret;
	ret.min = glm::min(b1.min, b2.min);
	ret.max = glm::max(b1.max, b2.max);
	return ret;
}

Bounds3f Union(const Bounds3f& b, glm::vec3 p) {
	Bounds3f ret;
	ret.min = glm::min(b.min, p);
	ret.max = glm::max(b.max, p);
	return ret;
}

Bounds3i Union(const Bounds3i& b1, const Bounds3i& b2) {
	Bounds3i ret;
	ret.min = glm::min(b1.min, b2.min);
	ret.max = glm::max(b1.max, b2.max);
	return ret;
}

bool Inside(glm::vec3 p, const Bounds3f& b) {
	return (p.x >= b.min.x && p.x <= b.max.x && p.y >= b.min.y && p.y <= b.max.y && p.z >= b.min.z && p.z <= b.max.z);
}
