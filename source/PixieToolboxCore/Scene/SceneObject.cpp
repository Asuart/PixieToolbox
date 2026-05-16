#include "SceneObject.h"

#include "Component.h"

SceneObject::SceneObject(const std::string& name, Transform transform) :
	m_name(name), m_localTransform(transform), m_parent(nullptr) {
}

const std::vector<Component*>& SceneObject::GetComponents() const {
	return m_components;
}

Component* SceneObject::GetComponent(int32_t index) const {
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

const std::string SceneObject::GetName() const {
	return m_name;
}

void SceneObject::SetName(const std::string& name) {
	m_name = name;
}

Transform& SceneObject::GetTransform() {
	return m_localTransform;
}

const Transform& SceneObject::GetTransform() const {
	return m_localTransform;
}

const std::vector<SceneObject*>& SceneObject::GetChildren() const {
	return m_children;
}

SceneObject* SceneObject::GetChild(int32_t index) const {
	return m_children[index];
}

SceneObject* SceneObject::FindObject(const std::string& objectName) const {
	for (size_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->m_name == objectName) {
			return m_children[i];
		}
	}
	return nullptr;
}

std::vector<SceneObject*> SceneObject::FindObjects(const std::string& objectName) const {
	std::vector<SceneObject*> objects;
	for (size_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->m_name == objectName) {
			objects.push_back(m_children[i]);
		}
		std::vector<SceneObject*> childrenFound = m_children[i]->FindObjects(objectName);
		objects.insert(objects.end(), childrenFound.begin(), childrenFound.end());
	}
	return objects;
}
