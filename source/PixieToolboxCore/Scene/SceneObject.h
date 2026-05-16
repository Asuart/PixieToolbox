#pragma once
#include <string>

#include "Math/Transform.h"
#include "Component.h"

class Component;

class SceneObject {
public:
	void OnStart();
	void OnUpdate();
	void OnFixedUpdate();

	SceneObject(const std::string& name, Transform transform = Transform());

	const std::string GetName() const;
	void SetName(const std::string& name);
	const std::vector<SceneObject*>& GetChildren() const;
	SceneObject* GetChild(int32_t index) const;
	const std::vector<Component*>& GetComponents() const;
	Component* GetComponent(int32_t index) const;
	Transform& GetTransform();
	const Transform& GetTransform() const;

	SceneObject* FindObject(const std::string& objectName) const;
	std::vector<SceneObject*> FindObjects(const std::string& objectName) const;

protected:
	std::string m_name;
	Transform m_localTransform;
	Transform m_worldTransform;
	SceneObject* m_parent;
	std::vector<SceneObject*> m_children;
	std::vector<Component*> m_components;

	friend class Scene;

public:
	template<typename T>
	T* GetComponent() const {
		uint64_t type = typeid(T).hash_code();
		for (size_t i = 0; i < m_components.size(); i++) {
			if (type == typeid(*m_components[i]).hash_code()) {
				return dynamic_cast<T*>(m_components[i]);
			}
		}
		return nullptr;
	}

	template<typename T>
	const SceneObject* FindObjectWithComponent() const {
		if (GetComponent<T>()) {
			return this;
		}
		for (size_t i = 0; i < m_children.size(); i++) {
			if (const SceneObject* object = m_children[i]->FindObjectWithComponent<T>()) {
				return object;
			}
		}
		return nullptr;
	}

	template<typename T>
	void FindObjectsWithComponent(std::vector<const SceneObject*>& objects) const {
		if (GetComponent<T>()) {
			objects.push_back(this);
		}
		for (size_t i = 0; i < m_children.size(); i++) {
			m_children[i]->FindObjectsWithComponent<T>(objects);
		}
	}
};
