#pragma once
#include "MathBase.h"
#include "Bounds.h"

class Transform {
public:
	Transform() = default;
	Transform(const glm::vec3& position, const glm::quat& rotation = glm::quat(), const glm::vec3& scale = glm::vec3(1.0f));
	Transform(const glm::mat4& m);
	Transform(const glm::mat4& m, const glm::mat4& mInv);

	const glm::mat4& GetMatrix() const;
	const glm::mat4& GetInverseMatrix() const;
	glm::vec3 GetPosition() const;
	glm::quat GetRotation() const;
	glm::vec3 GetEulerRotation() const;
	glm::vec3 GetScale() const;
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetForward() const;

	void Set(const Transform& transform);
	void Set(glm::mat4 transform);
	void Set(const glm::vec3& position, const glm::quat& rotation = glm::quat(), const glm::vec3& scale = glm::vec3(1.0f));
	void LookAt(glm::vec3 pos, glm::vec3 look, glm::vec3 up);
	void SetPosition(const glm::vec3& pos);
	void Translate(const glm::vec3& offset);
	void SetRotation(const glm::quat& rotation);
	void SetEulerRotation(glm::vec3 degrees);
	void Rotate(const glm::quat& rotation);
	void Rotate(float x, float y, float z);
	void RotateX(float radians);
	void RotateY(float radians);
	void RotateZ(float radians);
	void RotateAroundAxis(glm::vec3 axis, float radians);
	void SetScale(const glm::vec3& scale);
	void Scale(const glm::vec3& change);
	void Invert();

	glm::vec3 ApplyPoint(glm::vec3 p) const;
	glm::vec3 ApplyVector(glm::vec3 v) const;
	glm::vec3 ApplyNormal(glm::vec3 n) const;
	Bounds3f ApplyBounds(const Bounds3f& b) const;

	glm::vec3 ApplyInversePoint(glm::vec3 p) const;
	glm::vec3 ApplyInverseVector(glm::vec3 v) const;
	glm::vec3 ApplyInverseNormal(glm::vec3 n) const;

	bool IsIdentity() const;
	bool HasScale(float tolerance = 1e-3f) const;
	bool SwapsHandedness() const;

	bool operator==(const Transform& t) const;
	bool operator!=(const Transform& t) const;
	Transform operator*(const Transform& t2) const;

protected:
	glm::mat4 m_transform = glm::mat4(1.0f);
	glm::mat4 m_inverseTransform = glm::mat4(1.0f);
};

Transform Inverse(const Transform& t);
Transform Transpose(const Transform& t);
Transform RotateFromTo(glm::vec3 from, glm::vec3 to);
Transform RotateAroundAxis(float sinTheta, float cosTheta, glm::vec3 axis);
Transform RotateAroundAxis(float theta, glm::vec3 axis);
Transform LookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up);
