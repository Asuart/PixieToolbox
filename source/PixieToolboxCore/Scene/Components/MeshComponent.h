#pragma once
#include "Scene/Component.h"
#include "Rendering/ResourceHandles.h"

class MeshComponent : public Component {
public:
	MeshComponent(SceneObject* parent, MeshHandle mesh);

	MeshHandle GetMesh() const;
	void SetMesh(MeshHandle mesh);

protected:
	MeshHandle m_mesh;
};
