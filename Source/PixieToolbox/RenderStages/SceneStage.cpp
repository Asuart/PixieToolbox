#include "SceneStage.h"

#include <unordered_set>

#include <PixieRenderer/Material/IMaterial.h>
#include <PixieRenderer/RenderGraph/RenderGraphBuilder.h>
#include <PixieRenderer/RenderGraph/RenderGraphContext.h>
#include <PixieRenderer/Renderer/IRenderer.h>

#include <PixieToolboxCore/Scene/Components.h>
#include <PixieToolboxCore/Scene/Scene.h>

namespace PixieToolbox {

SceneStage::SceneStage(std::shared_ptr<IRenderer> renderer, RGResource output, glm::uvec2 res)
    : IRenderStage(renderer), m_output(output), m_resolution(res) {
	m_cameraUBO = renderer->CreateBuffer(BufferType::Uniform, sizeof(CameraUBO));
	m_cameraPositionUBO = renderer->CreateBuffer(BufferType::Uniform, sizeof(glm::vec4));
}

std::string_view SceneStage::GetName() const {
	return "Scene";
}

StageType SceneStage::GetType() const {
	return StageType::Graphics;
}

void SceneStage::Declare(RenderGraphBuilder& b) {
	b.Write(m_output, ResourceUsage::ColorAttachment);
	b.SetRenderTarget(m_output);
}

void SceneStage::Compile(RenderGraphContext&) {
}

void SceneStage::BindResources(RenderGraphContext& ctx) {
	if (!m_scene) {
		return;
	}

	std::shared_ptr<IRenderer> r = ctx.GetRenderer();

	glm::mat4 view(1.0f);
	glm::vec3 camPos(0.0f);
	bool foundCamera = false;

	Scene::Entity camEntity = m_scene->FindEntity("MainCamera");
	if (camEntity != Scene::Null) {
		if (auto* tc = m_scene->TryGetComponent<TransformComponent>(camEntity)) {
			camPos = tc->transform.GetPosition();
			view = glm::lookAt(camPos, camPos + tc->transform.GetForward(), tc->transform.GetUp());
			foundCamera = true;
		}
	}
	if (!foundCamera) {
		view = glm::lookAt(glm::vec3(0.0f, 1.5f, -4.0f), glm::vec3(0.0f), glm::vec3(0, 1, 0));
	}

	CameraUBO camData{};
	camData.view = view;
	const float aspect = static_cast<float>(m_resolution.x) / static_cast<float>(m_resolution.y);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5000.0f);
	camData.projection = proj;

	r->UpdateBuffer(m_cameraUBO, std::as_bytes(std::span{ &camData, 1 }));

	const glm::vec4 camPos4(camPos, 1.0f);
	r->UpdateBuffer(m_cameraPositionUBO, std::as_bytes(std::span{ &camPos4, 1 }));

	entt::registry& reg = m_scene->Registry();
	std::unordered_set<IMaterial*> bound;

	auto matView = reg.view<MaterialComponent>();
	for (auto e : matView) {
		MaterialComponent& mc = matView.get<MaterialComponent>(e);
		if (!mc.material || !mc.materialHandle) {
			continue;
		}
		if (!bound.insert(mc.material.get()).second) {
			continue;
		}

		r->BindBuffer(mc.materialHandle, "camera", m_cameraUBO);
		r->BindBuffer(mc.materialHandle, "cameraPosition", m_cameraPositionUBO);
		mc.material->Bind(r);
	}
}

void SceneStage::Execute(RenderGraphContext& ctx) {
	if (!m_scene) {
		return;
	}

	std::shared_ptr<IRenderer> r = ctx.GetRenderer();
	entt::registry& reg = m_scene->Registry();

	auto drawView = reg.view<MeshComponent, MaterialComponent, WorldMatrixComponent>();
	drawView.each([&](entt::entity /*e*/, MeshComponent& meshc, MaterialComponent& matc, WorldMatrixComponent& wtc) {
		if (!meshc.meshHandle || !matc.materialHandle) {
			return;
		}

		DrawRequest req{};
		req.material = matc.materialHandle;
		req.mesh = meshc.meshHandle;
		req.inlineData = std::as_bytes(std::span{ &wtc.matrix, 1 });
		r->DrawMesh(req);
	});
}

void SceneStage::SetScene(std::shared_ptr<Scene> scene) {
	m_scene = scene;
}

} // namespace PixieToolbox
