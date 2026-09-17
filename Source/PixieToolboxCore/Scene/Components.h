#pragma once
#include <memory>
#include <string>
#include <vector>

#include <entt/entt.hpp>

#include <PixieRenderer/Material/IMaterial.h>
#include <PixieRenderer/Mesh/Mesh.h>
#include <PixieRenderer/ResourceManager/ResourceHandles.h>

#include "Camera.h"
#include "PixieToolboxCore/Math/Transform.h"
#include "PixieToolboxCore/Scripts/Script.h"

using PixieRenderer::IMaterial;
using PixieRenderer::MaterialHandle;
using PixieRenderer::Mesh;
using PixieRenderer::MeshHandle;

namespace PixieToolbox {

struct NameComponent {
	std::string name;
};

struct HierarchyComponent {
	entt::entity parent = entt::null;
	std::vector<entt::entity> children;
};

struct TransformComponent {
	Transform transform;
};

struct MeshComponent {
	Mesh* mesh;
	MeshHandle meshHandle;
};

struct MaterialComponent {
	IMaterial* material;
	MaterialHandle materialHandle;
};

struct SphereComponent {
	float radius = 1.0f;
};

struct CameraComponent {
	Camera camera;
};

struct ScriptComponent {
	std::vector<std::unique_ptr<Script>> scripts;

	ScriptComponent() = default;

	ScriptComponent(const ScriptComponent&) = delete;
	ScriptComponent& operator=(const ScriptComponent&) = delete;

	ScriptComponent(ScriptComponent&&) noexcept = default;
	ScriptComponent& operator=(ScriptComponent&&) noexcept = default;

};

} // namespace PixieToolbox
