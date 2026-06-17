#include "MeshComponent.h"

MeshComponent::MeshComponent(std::shared_ptr<SceneObject> parent, MeshHandle mesh) :
	Component(parent), m_mesh(mesh) {
}

MeshHandle MeshComponent::GetMesh() const {
	return m_mesh;
}

void MeshComponent::SetMesh(MeshHandle mesh) {
	m_mesh = mesh;
}
