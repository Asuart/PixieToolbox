#pragma once

#include <memory>
class SceneObject;

class Component {
public:
	Component() = default;
	Component(std::shared_ptr<SceneObject> parent);

	std::shared_ptr<SceneObject> GetParent();

	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();

protected:
	std::shared_ptr<SceneObject> m_parent;
};
