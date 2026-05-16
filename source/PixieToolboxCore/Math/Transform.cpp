#include "Transform.h"

struct Decomposition {
	glm::vec3 scale;
	glm::quat orientation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
};

inline Decomposition Decompose(const glm::mat4& m) {
	Decomposition dec;
	glm::decompose(m, dec.scale, dec.orientation, dec.translation, dec.skew, dec.perspective);
	return dec;
}

inline glm::mat4 Recompose(const Decomposition& dec) {
	return glm::recompose(dec.scale, dec.orientation, dec.translation, dec.skew, dec.perspective);
}

Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
	Set(position, rotation, scale);
};

Transform::Transform(const glm::mat4& m) :
	m_transform(m), m_inverseTransform(glm::inverse(m)) {
}

Transform::Transform(const glm::mat4& m, const glm::mat4& mInv) :
	m_transform(m), m_inverseTransform(mInv) {
}

const glm::mat4& Transform::GetMatrix() const {
	return m_transform;
}

const glm::mat4& Transform::GetInverseMatrix() const {
	return m_inverseTransform;
}

glm::vec3 Transform::GetPosition() const {
	Decomposition dec = Decompose(m_transform);
	return dec.translation;
}

glm::quat Transform::GetRotation() const {
	Decomposition dec = Decompose(m_transform);
	return dec.orientation;
}

glm::vec3 Transform::GetEulerRotation() const {
	Decomposition dec = Decompose(m_transform);
	return glm::degrees(glm::eulerAngles(dec.orientation));
}

glm::vec3 Transform::GetScale() const {
	Decomposition dec = Decompose(m_transform);
	return dec.scale;
}

glm::vec3 Transform::GetRight() const {
	Decomposition dec = Decompose(m_transform);
	return glm::rotate(dec.orientation, glm::vec3(1, 0, 0));
}

glm::vec3 Transform::GetUp() const {
	Decomposition dec = Decompose(m_transform);
	return glm::rotate(dec.orientation, glm::vec3(0, 1, 0));
}

glm::vec3 Transform::GetForward() const {
	Decomposition dec = Decompose(m_transform);
	return glm::rotate(dec.orientation, glm::vec3(0, 0, -1));
}

void Transform::Set(glm::mat4 transform) {
	m_transform = transform;
	m_inverseTransform = glm::inverse(transform);
}

void Transform::Set(const Transform& transform) {
	m_transform = transform.m_transform;
	m_inverseTransform = transform.m_inverseTransform;
}

void Transform::Set(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
	m_transform = glm::mat4(rotation);
	m_transform = glm::scale(m_transform, scale);
	m_transform = glm::translate(m_transform, position);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::LookAt(glm::vec3 pos, glm::vec3 look, glm::vec3 up) {
	m_inverseTransform = glm::lookAt(pos, look, up);
	m_transform = glm::inverse(m_inverseTransform);
}

void Transform::SetPosition(const glm::vec3& pos) {
	Decomposition dec = Decompose(m_transform);
	dec.translation = pos;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::Translate(const glm::vec3& offset) {
	Decomposition dec = Decompose(m_transform);
	dec.translation += offset;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::SetRotation(const glm::quat& rotation) {
	Decomposition dec = Decompose(m_transform);
	dec.orientation = rotation;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::SetEulerRotation(glm::vec3 degrees) {
	Decomposition dec = Decompose(m_transform);
	dec.orientation = glm::quat(glm::radians(degrees));
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::Rotate(const glm::quat& rotation) {
	Decomposition dec = Decompose(m_transform);
	dec.orientation *= rotation;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::Rotate(float x, float y, float z) {
	Decomposition dec = Decompose(m_transform);
	dec.orientation *= glm::quat({ x, y ,z });
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::RotateX(float radians) {
	Rotate(radians, 0, 0);
}

void Transform::RotateY(float radians) {
	Rotate(0, radians, 0);
}

void Transform::RotateZ(float radians) {
	Rotate(0, 0, radians);
}

void Transform::RotateAroundAxis(glm::vec3 axis, float radians) {
	Decomposition dec = Decompose(m_transform);
	dec.orientation = glm::rotate(dec.orientation, radians, axis);
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::SetScale(const glm::vec3& scale) {
	Decomposition dec = Decompose(m_transform);
	dec.scale = scale;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::Scale(const glm::vec3& change) {
	Decomposition dec = Decompose(m_transform);
	dec.scale += change;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::Invert() {
	std::swap(m_transform, m_inverseTransform);
}

glm::vec3 Transform::ApplyPoint(glm::vec3 p) const {
	glm::vec4 transformed = m_transform * glm::vec4(p, 1.0);
	if (transformed.w == 1.0f) {
		return transformed;
	}
	else {
		return transformed / transformed.w;
	}
}

glm::vec3 Transform::ApplyVector(glm::vec3 v) const {
	return m_transform * glm::vec4(v, 0.0);
}

glm::vec3 Transform::ApplyNormal(glm::vec3 n) const {
	return glm::mat3(glm::transpose(glm::inverse(m_transform))) * n;
}

Bounds3f Transform::ApplyBounds(const Bounds3f& b) const {
	Bounds3f bt;
	for (int32_t i = 0; i < 8; ++i) {
		bt = Union(bt, ApplyPoint(b.Corner(i)));
	}
	return bt;
}

glm::vec3 Transform::ApplyInversePoint(glm::vec3 p) const {
	glm::vec4 transformed = m_inverseTransform * glm::vec4(p, 1.0);
	if (transformed.w == 1.0f) {
		return transformed;
	}
	else {
		return transformed / transformed.w;
	}
}

glm::vec3 Transform::ApplyInverseVector(glm::vec3 v) const {
	return m_inverseTransform * glm::vec4(v, 0.0);
}

glm::vec3 Transform::ApplyInverseNormal(glm::vec3 n) const {
	return glm::mat3(glm::transpose(glm::inverse(m_inverseTransform))) * n;
}

bool Transform::IsIdentity() const {
	return m_transform == glm::mat4(1.0f);
}

bool Transform::HasScale(float tolerance) const {
	float la2 = length2(ApplyVector(glm::vec3(1, 0, 0)));
	float lb2 = length2(ApplyVector(glm::vec3(0, 1, 0)));
	float lc2 = length2(ApplyVector(glm::vec3(0, 0, 1)));
	return (std::abs(la2 - 1) > tolerance || std::abs(lb2 - 1) > tolerance || std::abs(lc2 - 1) > tolerance);
}

bool Transform::SwapsHandedness() const {
	return glm::determinant(glm::mat3(m_transform)) < 0;
}

bool Transform::operator==(const Transform& t) const {
	return t.m_transform == m_transform;
}

bool Transform::operator!=(const Transform& t) const {
	return t.m_transform != m_transform;
}

Transform Transform::operator*(const Transform& t2) const {
	return Transform(m_transform * t2.m_transform, t2.m_inverseTransform * m_inverseTransform);
}

Transform Inverse(const Transform& t) {
	return Transform(t.GetInverseMatrix(), t.GetMatrix());
}

Transform Transpose(const Transform& t) {
	return Transform(glm::transpose(t.GetMatrix()), glm::transpose(t.GetInverseMatrix()));
}

Transform RotateFromTo(glm::vec3 from, glm::vec3 to) {
	glm::vec3 refl;
	if (std::abs(from.x) < 0.72f && std::abs(to.x) < 0.72f) {
		refl = glm::vec3(1, 0, 0);
	}
	else if (std::abs(from.y) < 0.72f && std::abs(to.y) < 0.72f) {
		refl = glm::vec3(0, 1, 0);
	}
	else {
		refl = glm::vec3(0, 0, 1);
	}

	glm::vec3 u = refl - from, v = refl - to;
	glm::mat4 r;
	for (int32_t i = 0; i < 3; ++i) {
		for (int32_t j = 0; j < 3; ++j) {
			r[i][j] = ((i == j) ? 1 : 0) - 2 / glm::dot(u, u) * u[i] * u[j] -
				2 / glm::dot(v, v) * v[i] * v[j] +
				4 * glm::dot(u, v) / (glm::dot(u, u) * glm::dot(v, v)) * v[i] * u[j];
		}
	}

	return Transform(r, glm::transpose(r));
}

Transform RotateAroundAxis(float sinTheta, float cosTheta, glm::vec3 axis) {
	glm::vec3 a = glm::normalize(axis);
	glm::mat4 m = glm::mat4(1.0f);
	m[0][0] = a.x * a.x + ((float)1 - a.x * a.x) * cosTheta;
	m[1][0] = a.x * a.y * ((float)1 - cosTheta) - a.z * sinTheta;
	m[2][0] = a.x * a.z * ((float)1 - cosTheta) + a.y * sinTheta;
	m[3][0] = 0;

	m[0][1] = a.x * a.y * ((float)1 - cosTheta) + a.z * sinTheta;
	m[1][1] = a.y * a.y + ((float)1 - a.y * a.y) * cosTheta;
	m[2][1] = a.y * a.z * ((float)1 - cosTheta) - a.x * sinTheta;
	m[3][1] = 0;

	m[0][2] = a.x * a.z * ((float)1 - cosTheta) - a.y * sinTheta;
	m[1][2] = a.y * a.z * ((float)1 - cosTheta) + a.x * sinTheta;
	m[2][2] = a.z * a.z + ((float)1 - a.z * a.z) * cosTheta;
	m[3][2] = 0;

	return Transform(m);
}

Transform RotateAroundAxis(float theta, glm::vec3 axis) {
	float sinTheta = std::sin(glm::radians(theta));
	float cosTheta = std::cos(glm::radians(theta));
	return RotateAroundAxis(sinTheta, cosTheta, axis);
}

Transform LookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up) {
	Transform t;
	t.LookAt(from, to, up);
	return t;
}
