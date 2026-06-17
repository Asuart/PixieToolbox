#pragma once
#include <cstdint>
#include <cmath>
#include <numeric>
#include <numbers>
#include <limits>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

static constexpr float Pi = static_cast<float>(std::numbers::pi);
static constexpr float TwoPi = Pi * 2.0f;
static constexpr float FourPi = Pi * 4.0f;
static constexpr float InvPi = static_cast<float>(std::numbers::inv_pi);
static constexpr float Inv2Pi = 1.0f / TwoPi;
static constexpr float Inv4Pi = 1.0f / FourPi;
static constexpr float PiOver2 = Pi / 2.0f;
static constexpr float PiOver4 = Pi / 4.0f;
static constexpr float Sqrt2 = static_cast<float>(std::numbers::sqrt2);
static constexpr float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5f;
static constexpr float Infinity = std::numeric_limits<float>::infinity();
static constexpr float ShadowEpsilon = 0.0001f;
static constexpr float MaxDegrees = 360.0f;
static constexpr float minFloat = std::numeric_limits<float>::lowest();
static constexpr float maxFloat = std::numeric_limits<float>::max();
static constexpr float FloatOneMinusEpsilon = float(0x1.fffffep-1);
static constexpr double DoubleOneMinusEpsilon = 0x1.fffffffffffffp-1f;
static constexpr float OneMinusEpsilon = FloatOneMinusEpsilon;

inline bool isnan(const glm::vec2& v) {
	return std::isnan(v.x) || std::isnan(v.y);
}

inline bool isnan(const glm::vec3& v) {
	return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}

constexpr int32_t Lerp(float t, int32_t from, int32_t to) {
	return int32_t(from + (to - from) * t);
}

constexpr uint32_t Lerp(float t, uint32_t from, uint32_t to) {
	return uint32_t(from + (to - from) * t);
}

constexpr float Lerp(float t, float from, float to) {
	return float(from + (to - from) * t);
}

constexpr glm::vec2 Lerp(float t, glm::vec2 from, glm::vec2 to) {
	return from + (to - from) * t;
}

constexpr glm::vec3 Lerp(float t, glm::vec3 from, glm::vec3 to) {
	return from + (to - from) * t;
}

constexpr glm::ivec2 Lerp(float t, glm::ivec2 from, glm::ivec2 to) {
	return glm::ivec2(glm::vec2(from) + glm::vec2(to - from) * t);
}

constexpr glm::ivec3 Lerp(float t, glm::ivec3 from, glm::ivec3 to) {
	return glm::ivec3(glm::vec3(from) + glm::vec3(to - from) * t);
}

template<typename T>
constexpr T Sqr(T v) {
	return v * v;
}

template <typename T, typename U, typename V>
constexpr T Clamp(T val, U low, V high) {
	if (val < low)       return T(low);
	else if (val > high) return T(high);
	else                 return val;
}

inline float MaxComponent(const glm::vec3& v) {
	return glm::max(v.x, glm::max(v.y, v.z));
}

inline int32_t MaxComponentIndex(const glm::vec3& d) {
	return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z ? 1 : 2);
}

inline float SafeSqrt(float v) {
	return glm::sqrt(glm::max(0.0f, v));
}

inline float AbsDot(const glm::vec3& v1, const glm::vec3& v2) {
	return glm::abs(glm::dot(v1, v2));
}

inline float Length2(const glm::vec2& v) {
	return v.x * v.x + v.y * v.y;
}

inline float Length2(const glm::vec3& v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline void CoordinateSystem(glm::vec3 v1, glm::vec3* v2, glm::vec3* v3) {
	float sign = std::copysign(1.0f, v1.z);
	float a = -1 / (sign + v1.z);
	float b = v1.x * v1.y * a;
	*v2 = glm::vec3(1.0f + sign * Sqr(v1.x) * a, sign * b, -sign * v1.x);
	*v3 = glm::vec3(b, sign + Sqr(v1.y) * a, -v1.y);
}

inline float Aspect(glm::ivec2 resolution) {
	return static_cast<float>(resolution.x) / static_cast<float>(resolution.y);
}
