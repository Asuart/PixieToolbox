#include "SceneObject.h"

#include <glaze/glaze.hpp>
#include <memory>

#include "Component.h"

SceneObject::SceneObject(const std::string& name) :
	m_name(name) {}

std::shared_ptr<SceneObject> SceneObject::GetParent() {
    return m_parent.lock();
}

void SceneObject::SetParent(const std::shared_ptr<SceneObject>& object) {
    std::shared_ptr<SceneObject> me = shared_from_this();
    std::shared_ptr<SceneObject> parent = m_parent.lock();
    if (parent) {
        auto it = std::find(parent->m_children.begin(), parent->m_children.end(), me);
        if (it != parent->m_children.end()) {
            parent->m_children.erase(it);
        }
    }
    parent = object;
    if (parent) {
        parent->m_children.push_back(me);
    }
}

const std::vector<std::shared_ptr<Component>>& SceneObject::GetComponents() const {
	return m_components;
}

std::shared_ptr<Component> SceneObject::GetComponent(int32_t index) const {
	return m_components[index];
}

void SceneObject::OnStart() {
	for (size_t i = 0; i < m_components.size(); i++) {
		m_components[i]->OnStart();
	}
	for (size_t i = 0; i < m_children.size(); i++) {
		m_children[i]->OnStart();
	}
}

void SceneObject::OnUpdate() {
	for (size_t i = 0; i < m_components.size(); i++) {
		m_components[i]->OnUpdate();
	}
	for (size_t i = 0; i < m_children.size(); i++) {
		m_children[i]->OnUpdate();
	}
}

void SceneObject::OnFixedUpdate() {
	for (size_t i = 0; i < m_components.size(); i++) {
		m_components[i]->OnFixedUpdate();
	}
	for (size_t i = 0; i < m_children.size(); i++) {
		m_children[i]->OnFixedUpdate();
	}
}

const std::vector<std::shared_ptr<SceneObject>>& SceneObject::GetChildren() const {
	return m_children;
}

std::shared_ptr<SceneObject> SceneObject::GetChild(int32_t index) const {
	return m_children[index];
}

std::shared_ptr<SceneObject> SceneObject::FindObject(const std::string& objectName) const {
	for (size_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->m_name == objectName) {
			return m_children[i];
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<SceneObject>> SceneObject::FindObjects(const std::string& objectName) const {
	std::vector<std::shared_ptr<SceneObject>> objects;
	for (size_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->m_name == objectName) {
			objects.push_back(m_children[i]);
		}
		std::vector<std::shared_ptr<SceneObject>> childrenFound = m_children[i]->FindObjects(objectName);
		objects.insert(objects.end(), childrenFound.begin(), childrenFound.end());
	}
	return objects;
}
