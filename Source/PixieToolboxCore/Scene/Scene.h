#pragma once

#include <vector>
#include <memory>

#include <ResourceManager.hpp>
#include <Resource.hpp>

#include "SceneObject.h"

class Scene {
	GENERATE_RESOURCEABLE(Scene,
        std::shared_ptr<SceneObject> m_rootObject;
    )
public:
	Scene() = default;
	Scene(const std::string& name);
	~Scene();

	void Start();
	void Update();
	void FixedUpdate();

	const std::string& GetName() const;
	void SetName(const std::string& name);
	std::shared_ptr<SceneObject> GetRootObject() const;

	void AddObject(std::shared_ptr<SceneObject> object, std::shared_ptr<SceneObject> parent = nullptr);
	std::shared_ptr<SceneObject> CreateObject(const std::string& name, std::shared_ptr<SceneObject> parent = nullptr);
	void RemoveObject(const std::string& objectName);
	void RemoveObjects(const std::string& objectName);
	void RemoveObject(const std::shared_ptr<SceneObject> object);
	void RemoveObjects(const std::vector<std::shared_ptr<SceneObject>>& objects);
	std::shared_ptr<SceneObject> FindObject(const std::string& objectName) const;
	std::vector<std::shared_ptr<SceneObject>> FindObjects(const std::string& objectName) const;

	std::vector<std::byte> serialize();
	static Scene* deserialize(const std::vector<std::byte>&);

protected:
	std::vector<std::shared_ptr<SceneObject>> m_flatObjects;
	std::vector<std::shared_ptr<Component>> m_flatComponents;

public:
    template<typename T, typename ...Args>
    void CreateComponent(std::shared_ptr<SceneObject> object, Args... args) {
        std::shared_ptr<T> component = std::make_shared<T>(object, args...);
        object->m_components.push_back(component);
    }
    
	template<typename T>
	std::shared_ptr<SceneObject> FindObjectWithComponent() {
		return _data.m_rootObject->FindObjectWithComponent<T>();
	}

	template<typename T>
	std::vector<std::shared_ptr<SceneObject>> FindObjectsWithComponent() {
		std::vector<std::shared_ptr<SceneObject>> objects;
		_data.m_rootObject->FindObjectsWithComponent<T>(objects);
		return objects;
	}
};
