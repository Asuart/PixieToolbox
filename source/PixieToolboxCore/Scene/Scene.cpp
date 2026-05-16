#include "Scene.h"

Scene::Scene(const std::string& name) :
	m_name(name) {
}

Scene::~Scene() {
	delete m_rootObject;
}

const std::string& Scene::GetName() const {
	return m_name;
}

void Scene::SetName(const std::string& name) {
	m_name = name;
}

SceneObject* Scene::FindObject(const std::string& objectName) const {
	return m_rootObject->FindObject(objectName);
}

std::vector<SceneObject*> Scene::FindObjects(const std::string& objectName) const {
	return m_rootObject->FindObjects(objectName);
}

SceneObject* Scene::GetRootObject() const {
	return m_rootObject;
}

Bounds3f Scene::GetBounds() const {
	return Bounds3f();
}

void Scene::Start() {
	m_rootObject->OnStart();
}

void Scene::Update() {
	m_rootObject->OnUpdate();
}

void Scene::FixedUpdate() {
	m_rootObject->OnFixedUpdate();
}

void Scene::AddObject(SceneObject* object, SceneObject* parent) {
	if (!parent || object == m_rootObject) return;
	if (object->m_parent) {
		for (int32_t i = 0; i < object->m_parent->m_children.size(); i++) {
			if (object->m_parent->m_children[i] == object) {
				object->m_parent->m_children.erase(object->m_parent->m_children.begin() + i);
				break;
			}
		}
	}
	object->m_parent = parent;
	parent->m_children.push_back(object);
}

SceneObject* Scene::CreateObject(const std::string& name, SceneObject* parent, const Transform& transform) {
	if (!parent) parent = m_rootObject;
	SceneObject* object = new SceneObject(name, transform);
	object->m_parent = parent;
	parent->m_children.push_back(object);
	return object;
}

void Scene::RemoveObject(const std::string& objectName) {
	SceneObject* object = FindObject(objectName);
	RemoveObject(object);
}

void Scene::RemoveObjects(const std::string& objectName) {
	std::vector<SceneObject*> objects = FindObjects(objectName);
	RemoveObjects(objects);
}

void Scene::RemoveObject(const SceneObject* object) {
	if (object == m_rootObject) return;
	if (object->m_parent) {
		for (int32_t i = 0; i < object->m_parent->m_children.size(); i++) {
			if (object->m_parent->m_children[i] == object) {
				object->m_parent->m_children.erase(object->m_parent->m_children.begin() + i);
				break;
			}
		}
	}
	delete object;
}

void Scene::RemoveObjects(const std::vector<SceneObject*>& objects) {
	for (int32_t i = 0; i < objects.size(); i++) {
		RemoveObject(objects[i]);
	}
}
