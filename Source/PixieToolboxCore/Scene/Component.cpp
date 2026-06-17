#include "Component.h"

#include "SceneObject.h"

Component::Component(std::shared_ptr<SceneObject> parent) :
	m_parent(parent) {
}

std::shared_ptr<SceneObject> Component::GetParent() {
	return m_parent;
}

void Component::OnStart() {}

void Component::OnUpdate() {}

void Component::OnFixedUpdate() {}
