#pragma once
#include <PixieRendering/ResourceHandles.h>

#include "PixieToolboxCore/Scene/Component.h"

class MeshComponent : public Component {
public:
	MeshComponent(std::shared_ptr<SceneObject> parent, MeshHandle mesh);

	MeshHandle GetMesh() const;
	void SetMesh(MeshHandle mesh);

protected:
	MeshHandle m_mesh;
};
