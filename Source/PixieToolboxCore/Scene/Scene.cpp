#include "Scene.h"

#include <algorithm>

#include "Components.h"
#include "PixieToolboxCOre/Scripts/Script.h"

namespace PixieToolbox {

Scene::Scene(const std::string& name) : m_name(name) {
	m_root = CreateEntity("root");
}

Scene::Entity Scene::CreateEntity(const std::string& name) {
	Entity e = m_registry.create();
	m_registry.emplace_or_replace<NameComponent>(e, NameComponent{ name });
	m_registry.emplace_or_replace<HierarchyComponent>(e);
	m_registry.emplace_or_replace<TransformComponent>(e);
	return e;
}

Scene::Entity Scene::CreateEntity(const std::string& name, Entity parent) {
	Entity e = CreateEntity(name);
	SetParent(e, parent);
	return e;
}

void Scene::DestroyEntity(Entity entity) {
	if (entity == Null || entity == m_root)
		return;
	if (!m_registry.valid(entity))
		return;

	SetParent(entity, Null);

	if (auto* h = m_registry.try_get<HierarchyComponent>(entity)) {
		std::vector<Entity> children = h->children;
		for (Entity c : children) {
			DestroyEntity(c);
		}
	}

	m_registry.destroy(entity);
}

void Scene::Clear() {
	m_registry.clear();
	m_root = CreateEntity("root");
}

void Scene::SetParent(Entity child, Entity parent) {
	if (child == Null || !m_registry.valid(child))
		return;
	if (child == parent)
		return;

	auto* h = m_registry.try_get<HierarchyComponent>(child);
	if (!h) {
		h = &m_registry.emplace_or_replace<HierarchyComponent>(child);
	}
	if (h->parent != Null) {
		if (auto* ph = m_registry.try_get<HierarchyComponent>(h->parent)) {
			auto& sib = ph->children;
			sib.erase(std::remove(sib.begin(), sib.end(), child), sib.end());
		}
	}
	h->parent = parent;

	if (parent != Null) {
		auto* ph = m_registry.try_get<HierarchyComponent>(parent);
		if (!ph) {
			ph = &m_registry.emplace_or_replace<HierarchyComponent>(parent);
		}
		ph->children.push_back(child);
	}
}

Scene::Entity Scene::GetParent(Entity entity) const {
	if (auto* h = m_registry.try_get<HierarchyComponent>(entity)) {
		return h->parent;
	}
	return Null;
}

const std::vector<Scene::Entity>& Scene::GetChildren(Entity entity) const {
	static const std::vector<Entity> empty;
	if (auto* h = m_registry.try_get<HierarchyComponent>(entity)) {
		return h->children;
	}
	return empty;
}

Scene::Entity Scene::FindEntity(const std::string& name) const {
	auto view = m_registry.view<NameComponent>();
	for (auto e : view) {
		if (view.get<NameComponent>(e).name == name)
			return e;
	}
	return Null;
}

std::vector<Scene::Entity> Scene::FindEntities(const std::string& name) const {
	std::vector<Entity> out;
	auto view = m_registry.view<NameComponent>();
	for (auto e : view) {
		if (view.get<NameComponent>(e).name == name)
			out.push_back(e);
	}
	return out;
}

void Scene::Start() {
	if (m_root == Null)
		return;
	StartEntity(m_root);
}

void Scene::Update() {
	if (m_root == Null)
		return;
	UpdateEntity(m_root);
}

void Scene::FixedUpdate() {
	if (m_root == Null)
		return;
	FixedUpdateEntity(m_root);
}

void Scene::StartEntity(Entity e) {
	if (auto* sc = m_registry.try_get<ScriptComponent>(e)) {
		for (auto& s : sc->scripts) {
			if (s)
				s->OnStart(*this, e);
		}
	}
	if (auto* h = m_registry.try_get<HierarchyComponent>(e)) {
		for (Entity c : h->children)
			StartEntity(c);
	}
}

void Scene::UpdateEntity(Entity e) {
	if (auto* sc = m_registry.try_get<ScriptComponent>(e)) {
		for (auto& s : sc->scripts) {
			if (s)
				s->OnUpdate(*this, e);
		}
	}
	if (auto* h = m_registry.try_get<HierarchyComponent>(e)) {
		for (Entity c : h->children)
			UpdateEntity(c);
	}
}

void Scene::FixedUpdateEntity(Entity e) {
	if (auto* sc = m_registry.try_get<ScriptComponent>(e)) {
		for (auto& s : sc->scripts) {
			if (s)
				s->OnFixedUpdate(*this, e);
		}
	}
	if (auto* h = m_registry.try_get<HierarchyComponent>(e)) {
		for (Entity c : h->children)
			FixedUpdateEntity(c);
	}
}

} // namespace PixieToolbox
