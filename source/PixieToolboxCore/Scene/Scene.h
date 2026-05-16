#pragma once
#include "Math/Transform.h"
#include "SceneObject.h"
#include "Camera.h"

class Scene {
public:
	Scene() = default;
	Scene(const std::string& name);
	~Scene();

	void Start();
	void Update();
	void FixedUpdate();

	const std::string& GetName() const;
	void SetName(const std::string& name);
	SceneObject* GetRootObject() const;
	Bounds3f GetBounds() const;

	void AddObject(SceneObject* object, SceneObject* parent = nullptr);
	SceneObject* CreateObject(const std::string& name, SceneObject* parent = nullptr, const Transform& transform = Transform());
	void RemoveObject(const std::string& objectName);
	void RemoveObjects(const std::string& objectName);
	void RemoveObject(const SceneObject* object);
	void RemoveObjects(const std::vector<SceneObject*>& objects);
	SceneObject* FindObject(const std::string& objectName) const;
	std::vector<SceneObject*> FindObjects(const std::string& objectName) const;

protected:
	std::string m_name = "New Scene";
	SceneObject* m_rootObject = new SceneObject("root");
	std::vector<SceneObject*> m_flatObjects;
	std::vector<Component*> m_flatComponents;

public:
	template<typename T>
	const SceneObject* FindObjectWithComponent() const {
		return m_rootObject->FindObjectWithComponent<T>();
	}

	template<typename T>
	std::vector<const SceneObject*> FindObjectsWithComponent() const {
		std::vector<const SceneObject*> objects;
		m_rootObject->FindObjectsWithComponent<T>(objects);
		return objects;
	}

	friend class SceneManager;
};
