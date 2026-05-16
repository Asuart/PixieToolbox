#include "Component.h"

#include "SceneObject.h"

Component::Component(SceneObject* parent) :
	m_parent(parent) {
}

SceneObject* Component::GetParent() {
	return m_parent;
}

void Component::OnStart() {}

void Component::OnUpdate() {}

void Component::OnFixedUpdate() {}
