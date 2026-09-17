#pragma once
#include <vector>

#include <entt/entt.hpp>

namespace PixieToolbox {

class Scene;

class Script {
  public:
	virtual ~Script() = default;

	virtual void OnStart(Scene& /*scene*/, entt::entity /*entity*/) {
	}
	virtual void OnDestroy(Scene& /*scene*/, entt::entity /*entity*/) {
	}

	virtual void OnEnable(Scene& /*scene*/, entt::entity /*entity*/) {
	}
	virtual void OnDisable(Scene& /*scene*/, entt::entity /*entity*/) {
	}

	virtual void OnUpdate(Scene& /*scene*/, entt::entity /*entity*/) {
	}
	virtual void OnFixedUpdate(Scene& /*scene*/, entt::entity /*entity*/) {
	}

	virtual void OnCollisionEnter(Scene& /*scene*/, entt::entity /*entity*/, entt::entity /*other*/) {
	}
	virtual void OnCollisionStay(Scene& /*scene*/, entt::entity /*entity*/, entt::entity /*other*/) {
	}
	virtual void OnCollisionExit(Scene& /*scene*/, entt::entity /*entity*/, entt::entity /*other*/) {
	}

	virtual void OnTriggerEnter(Scene& /*scene*/, entt::entity /*entity*/, entt::entity /*other*/) {
	}
	virtual void OnTriggerStay(Scene& /*scene*/, entt::entity /*entity*/, entt::entity /*other*/) {
	}
	virtual void OnTriggerExit(Scene& /*scene*/, entt::entity /*entity*/, entt::entity /*other*/) {
	}
};

} // namespace PixieToolbox
