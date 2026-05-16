#include "MeshComponent.h"

#include "Rendering/RenderEngine.h"

MeshComponent::MeshComponent(SceneObject* parent, MeshHandle mesh) :
	Component(parent), m_mesh(mesh) {
}

MeshHandle MeshComponent::GetMesh() const {
	return m_mesh;
}

void MeshComponent::SetMesh(MeshHandle mesh) {
	m_mesh = mesh;
}
