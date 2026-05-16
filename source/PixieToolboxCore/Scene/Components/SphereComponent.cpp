#include "SphereComponent.h"
#include "Rendering/RenderEngine.h"

SphereComponent::SphereComponent(SceneObject* parent, float radius) :
	Component(parent), m_radius(radius) {
}

float SphereComponent::GetRadius() const {
	return m_radius;
}

void SphereComponent::SetRadius(float radius) {
	m_radius = radius;
}
