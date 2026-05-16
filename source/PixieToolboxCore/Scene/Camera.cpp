#include "Camera.h"

Camera::Camera(glm::vec3 lookFrom, glm::vec3 lookAt, glm::vec3 up, float fovy, float aspect, float near, float far) :
	m_fovy(fovy), m_aspect(aspect), m_near(near), m_far(far) {
	LookAt(lookFrom, lookAt, up);
	UpdateProjection();
}

Camera::Camera(Transform transform, float fovy, float aspect, float near, float far) :
	m_transform(transform), m_fovy(fovy), m_aspect(aspect), m_near(near), m_far(far) {
	UpdateProjection();
}

void Camera::LookAt(const glm::vec3& lookFrom, const glm::vec3& lookAt, const glm::vec3& up) {
	m_transform.LookAt(lookFrom, lookAt, up);
}

float Camera::GetFieldOfViewY() const {
	return m_fovy;
}

void Camera::SetFieldOfViewY(float fovy) {
	m_fovy = fovy;
	UpdateProjection();
}

float Camera::GetAspect() const {
	return m_aspect;
}

void Camera::SetAspect(float aspect) {
	m_aspect = aspect;
	UpdateProjection();
}

float Camera::GetNear() const {
	return m_near;
}

void Camera::SetNear(float near) {
	m_near = near;
	UpdateProjection();
}

float Camera::GetFar() const {
	return m_far;
}

void Camera::SetFar(float far) {
	m_far = far;
	UpdateProjection();
}

Transform& Camera::GetTransform() {
	return m_transform;
}

const Transform& Camera::GetTransform() const {
	return m_transform;
}

void Camera::SetTransform(const Transform& transform) {
	m_transform = transform;
}

const glm::mat4& Camera::GetViewMatrix() const {
	return m_transform.GetInverseMatrix();
}

const glm::mat4& Camera::GetInverseViewMatrix() const {
	return m_transform.GetMatrix();
}

const glm::mat4& Camera::GetProjectionMatrix() const {
	return m_mProjection;
}

void Camera::UpdateProjection() {
	m_mProjection = glm::perspective(m_fovy, m_aspect, m_near, m_far);
}

bool Camera::operator!=(const Camera& other) {
	return m_mProjection != other.m_mProjection || m_transform != other.m_transform || m_aspect != other.m_aspect;
}

bool Camera::operator==(const Camera& other) {
	return m_mProjection == other.m_mProjection && m_transform == other.m_transform && m_aspect == other.m_aspect;
}
