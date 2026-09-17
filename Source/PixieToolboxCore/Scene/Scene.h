#pragma once
#include <memory>
#include <string>
#include <vector>

#include <entt/entt.hpp>

namespace PixieToolbox {

class Scene {
  public:
	using Entity = entt::entity;
	static constexpr Entity Null = entt::null;

	Scene() = default;
	Scene(const std::string& name);
	~Scene() = default;

	const std::string& GetName() const {
		return m_name;
	}
	void SetName(const std::string& name) {
		m_name = name;
	}

	entt::registry& Registry() {
		return m_registry;
	}
	const entt::registry& Registry() const {
		return m_registry;
	}

	Entity CreateEntity(const std::string& name = "Entity");
	Entity CreateEntity(const std::string& name, Entity parent);
	void DestroyEntity(Entity entity);
	void Clear();

	Entity GetRoot() const {
		return m_root;
	}
	void SetParent(Entity child, Entity parent);
	Entity GetParent(Entity entity) const;
	const std::vector<Entity>& GetChildren(Entity entity) const;

	template <typename T, typename... Args> T& AddComponent(Entity entity, Args&&... args) {
		return m_registry.emplace<T>(entity, std::forward<Args>(args)...);
	}

	template <typename T> T& GetComponent(Entity entity) {
		return m_registry.get<T>(entity);
	}

	template <typename T> const T& GetComponent(Entity entity) const {
		return m_registry.get<T>(entity);
	}

	template <typename T> T* TryGetComponent(Entity entity) {
		return m_registry.try_get<T>(entity);
	}

	template <typename T> bool HasComponent(Entity entity) const {
		return m_registry.all_of<T>(entity);
	}

	template <typename T> void RemoveComponent(Entity entity) {
		m_registry.remove<T>(entity);
	}

	template <typename T> auto View() {
		return m_registry.view<T>();
	}

	template <typename... T> auto View() {
		return m_registry.view<T...>();
	}

	Entity FindEntity(const std::string& name) const;
	std::vector<Entity> FindEntities(const std::string& name) const;

	template <typename T> Entity FindEntityWithComponent() const {
		auto view = m_registry.view<T>();
		auto it = view.begin();
		return it == view.end() ? Null : *it;
	}

	template <typename T> std::vector<Entity> FindEntitiesWithComponent() const {
		std::vector<Entity> out;
		auto view = m_registry.view<T>();
		for (auto e : view)
			out.push_back(e);
		return out;
	}

	void Start();
	void Update();
	void FixedUpdate();

  private:
	void StartEntity(Entity entity);
	void UpdateEntity(Entity entity);
	void FixedUpdateEntity(Entity entity);

	std::string m_name;
	entt::registry m_registry;
	Entity m_root = Null;
};

} // namespace PixieToolbox
