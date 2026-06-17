#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <typeinfo>
#include <memory>

#include "Component.h"

class Component;

class SceneObject : public std::enable_shared_from_this<SceneObject> {
public:
	void OnStart();
	void OnUpdate();
	void OnFixedUpdate();
    SceneObject() = default;
	SceneObject(const std::string& name);

    std::shared_ptr<SceneObject> GetParent();
    void SetParent(const std::shared_ptr<SceneObject>& object);

	const std::vector<std::shared_ptr<SceneObject>>& GetChildren() const;
	std::shared_ptr<SceneObject> GetChild(int32_t index) const;
	const std::vector<std::shared_ptr<Component>>& GetComponents() const;
	std::shared_ptr<Component> GetComponent(int32_t index) const;

	std::shared_ptr<SceneObject> FindObject(const std::string& objectName) const;
	std::vector<std::shared_ptr<SceneObject>> FindObjects(const std::string& objectName) const;

public:
    std::string m_name;
	std::vector<std::shared_ptr<SceneObject>> m_children;
	std::vector<std::shared_ptr<Component>> m_components;

private:
	std::weak_ptr<SceneObject> m_parent;

public:
	template<typename T>
	std::shared_ptr<T> GetComponent() const {
		uint64_t type = typeid(T).hash_code();
		for (size_t i = 0; i < m_components.size(); i++) {
			if (type == typeid(m_components[i]).hash_code()) {
				return dynamic_cast<T*>(m_components[i]);
			}
		}
		return nullptr;
	}

	template<typename T>
	std::shared_ptr<SceneObject> FindObjectWithComponent() {
		if (GetComponent<T>()) {
			return shared_from_this();
		}
		for (size_t i = 0; i < m_children.size(); i++) {
			if (const std::shared_ptr<SceneObject> object = m_children[i]->FindObjectWithComponent<T>()) {
				return object;
			}
		}
		return nullptr;
	}

	template<typename T>
	void FindObjectsWithComponent(std::vector<std::shared_ptr<SceneObject>>& objects) {
		if (GetComponent<T>()) {
			objects.push_back(shared_from_this());
		}
		for (size_t i = 0; i < m_children.size(); i++) {
			m_children[i]->FindObjectsWithComponent<T>(objects);
		}
	}
};
