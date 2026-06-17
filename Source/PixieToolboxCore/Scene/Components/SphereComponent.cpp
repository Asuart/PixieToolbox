#include "SphereComponent.h"

SphereComponent::SphereComponent(std::shared_ptr<SceneObject> parent, float radius) :
	Component(parent), m_radius(radius) {
}

float SphereComponent::GetRadius() const {
	return m_radius;
}

void SphereComponent::SetRadius(float radius) {
	m_radius = radius;
}
