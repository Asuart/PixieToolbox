#pragma once
#include "PixieToolboxCore/Math/MathBase.h"
#include "PixieToolboxCore/Math/Transform.h"

class Camera {
public:
	Camera(glm::vec3 lookFrom, glm::vec3 lookAt, glm::vec3 up, float fovy, float aspect, float _near = 0.01f, float _far = 1000.0);
	Camera(Transform transform, float fovy, float aspect, float _near = 0.01f, float _far = 1000.0);

	void LookAt(const glm::vec3& lookFrom, const glm::vec3& lookAt, const glm::vec3& up);
	float GetFieldOfViewY() const;
	void SetFieldOfViewY(float fovy);
	float GetAspect() const;
	void SetAspect(float aspect);
	float GetNear() const;
	void SetNear(float near);
	float GetFar() const;
	void SetFar(float far);

	Transform& GetTransform();
	const Transform& GetTransform() const;
	void SetTransform(const Transform& transform);
	const glm::mat4& GetViewMatrix() const;
	const glm::mat4& GetInverseViewMatrix() const;
	const glm::mat4& GetProjectionMatrix() const;

	bool operator!=(const Camera& other);
	bool operator==(const Camera& other);

protected:
	Transform m_transform;
	glm::mat4 m_mProjection = glm::mat4(1.0);
	float m_fovy = Pi / 2.0f;
	float m_aspect = 16.0f / 9.0f;
	float m_near = 0.01f;
	float m_far = 1000.0f;

	void UpdateProjection();
};
