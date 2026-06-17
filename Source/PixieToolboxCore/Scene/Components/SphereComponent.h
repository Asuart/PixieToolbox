#pragma once
#include "PixieToolboxCore/Scene/Component.h"

class SphereComponent : public Component {
public:
	SphereComponent(std::shared_ptr<SceneObject> parent, float radius);

	float GetRadius() const;
	void SetRadius(float radius);

protected:
	float m_radius;
};
