#pragma once
#include "Scene/Component.h"

class SphereComponent : public Component {
public:
	SphereComponent(SceneObject* parent, float radius);

	float GetRadius() const;
	void SetRadius(float radius);

protected:
	float m_radius;
};
