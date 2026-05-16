#pragma once

class SceneObject;

class Component {
public:
	Component() = delete;
	Component(SceneObject* parent);

	SceneObject* GetParent();

	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();

protected:
	SceneObject* m_parent;

	friend class Scene;
};
