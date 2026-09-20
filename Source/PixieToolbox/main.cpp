#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <PixieRenderer/ComputeProgram/BlurComputeProgram.h>
#include <PixieRenderer/Material/PBRMaterial.h>
#include <PixieRenderer/Material/PresentMaterial.h>
#include <PixieRenderer/PixieRenderer.h>
#include <PixieRenderer/RenderGraph/IRenderStage.h>
#include <PixieRenderer/RenderGraph/RenderGraph.h>
#include <PixieRenderer/RenderGraph/RenderGraphBuilder.h>
#include <PixieRenderer/RenderGraph/RenderGraphContext.h>
#include <PixieRenderer/Renderer/IRenderer.h>
#include <PixieRenderer/Renderer/Vulkan/RendererVulkan.h>
#include <PixieRenderer/Window/WindowVulkan.h>

#include <PixieToolboxCore/Time/ApplicationTime.h>
#include <PixieToolboxCore/UserInput/UserInput.h>

#include <PixieToolboxCore/Scene/Components.h>
#include <PixieToolboxCore/Scripts/FreeCameraController.h>
#include <PixieToolboxCore/Scene/Scene.h>

#include "PixieToolbox/UI/UI.h"
#include "PixieToolbox/UI/UIVulkan.h"
#include "PixieToolbox/UI/Windows/ApplicationStatsWindow.h"
#include "PixieToolbox/UI/Windows/DemoWindow.h"
#include "PixieToolbox/UI/Windows/TextureDisplayWindow.h"

#include <PixieToolboxCore/Scene/SceneLoader.h>

using namespace PixieRenderer;
using namespace PixieToolbox;

static constexpr glm::uvec2 kRenderSize = { 1280, 720 };
static constexpr const char* kCameraEntityName = "MainCamera";

// ============================================================================
// Shared camera data (namespace scope — used by SceneStage).
// ============================================================================

struct CameraUBO {
	glm::mat4 view;
	glm::mat4 projection;
};

// ============================================================================
// SceneStage
//
// Uses entt's registry directly (via Scene::Registry()) instead of
// Scene::View / Scene::GetComponent to avoid the single-arg template
// ambiguity in Scene.h. Also avoids CameraComponent because `Camera` in
// PixieToolboxCore/Scene/Camera.h has no default constructor — the camera
// entity is looked up by name and its transform is read instead.
// ============================================================================

class SceneStage : public IRenderStage {
  public:
	SceneStage(Scene& scene, RGResource output, BufferHandle camUBO, BufferHandle camPosUBO, glm::uvec2 res)
	    : m_scene(scene), m_output(output), m_cameraUBO(camUBO), m_cameraPositionUBO(camPosUBO), m_resolution(res) {
	}

	std::string_view GetName() const override {
		return "Scene";
	}
	StageType GetType() const override {
		return StageType::Graphics;
	}

	void Declare(RenderGraphBuilder& b) override {
		b.Write(m_output, ResourceUsage::ColorAttachment);
		b.SetRenderTarget(m_output);
	}
	void Compile(RenderGraphContext&) override {
	}

	void BindResources(RenderGraphContext& ctx) override {
		IRenderer* r = ctx.GetRenderer();

		// --- Camera: look up the entity by name, read its Transform. ---
		glm::mat4 view(1.0f);
		glm::vec3 camPos(0.0f);
		bool foundCamera = false;

		Scene::Entity camEntity = m_scene.FindEntity(kCameraEntityName);
		if (camEntity != Scene::Null) {
			if (auto* tc = m_scene.TryGetComponent<TransformComponent>(camEntity)) {
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
		proj[1][1] *= -1.0f; // Vulkan Y-flip
		camData.projection = proj;

		r->UpdateBuffer(m_cameraUBO, std::as_bytes(std::span{ &camData, 1 }));

		const glm::vec4 camPos4(camPos, 1.0f);
		r->UpdateBuffer(m_cameraPositionUBO, std::as_bytes(std::span{ &camPos4, 1 }));

		// --- Bind camera UBOs + per-material resources once per unique material. ---
		entt::registry& reg = m_scene.Registry();
		std::unordered_set<IMaterial*> bound;

		auto matView = reg.view<MaterialComponent>();
		for (auto e : matView) {
			MaterialComponent& mc = matView.get<MaterialComponent>(e);
			if (!mc.material || !mc.materialHandle)
				continue;
			if (!bound.insert(mc.material.get()).second)
				continue;

			r->BindBuffer(mc.materialHandle, "CameraUBO", m_cameraUBO);
			r->BindBuffer(mc.materialHandle, "CameraPosition", m_cameraPositionUBO);
			mc.material->Bind(r);
		}
	}

	void Execute(RenderGraphContext& ctx) override {
		IRenderer* r = ctx.GetRenderer();
		entt::registry& reg = m_scene.Registry();

		auto drawView = reg.view<MeshComponent, MaterialComponent, WorldMatrixComponent>();
		for (auto e : drawView) {
			MeshComponent& meshc = drawView.get<MeshComponent>(e);
			MaterialComponent& matc = drawView.get<MaterialComponent>(e);
			WorldMatrixComponent& wtc = drawView.get<WorldMatrixComponent>(e);

			if (!meshc.meshHandle || !matc.materialHandle)
				continue;

			DrawRequest req{};
			req.material = matc.materialHandle;
			req.mesh = meshc.meshHandle;
			req.inlineData = std::as_bytes(std::span{ &wtc.matrix, 1 });
			r->DrawMesh(req);
		}
	}

  private:
	Scene& m_scene;
	RGResource m_output;
	BufferHandle m_cameraUBO;
	BufferHandle m_cameraPositionUBO;
	glm::uvec2 m_resolution;
};

// ============================================================================
// BlurStage — box blur via compute shader.
// ============================================================================

class BlurStage : public IRenderStage {
  public:
	BlurStage(RGResource in, RGResource out, ComputeProgramHandle prog, glm::uvec2 size)
	    : m_input(in), m_output(out), m_prog(prog), m_size(size) {
	}
	std::string_view GetName() const override {
		return "Blur";
	}
	StageType GetType() const override {
		return StageType::Compute;
	}

	void Declare(RenderGraphBuilder& b) override {
		b.Read(m_input, ResourceUsage::StorageRead, "inputImage");
		b.Write(m_output, ResourceUsage::StorageWrite, "outputImage");
	}
	void Compile(RenderGraphContext&) override {
	}

	void BindResources(RenderGraphContext& ctx) override {
		ctx.BindInputs(m_prog);
	}

	void Execute(RenderGraphContext& ctx) override {
		struct Push {
			int32_t radius;
			int32_t p0, p1, p2;
		} pc{};
		pc.radius = 2;
		DispatchRequest req{};
		req.program = m_prog;
		req.x = (m_size.x + 7) / 8;
		req.y = (m_size.y + 7) / 8;
		req.z = 1;
		req.inlineData = std::as_bytes(std::span{ &pc, 1 });
		ctx.GetRenderer()->DispatchComputeProgram(req);
	}

  private:
	RGResource m_input, m_output;
	ComputeProgramHandle m_prog;
	glm::uvec2 m_size;
};

// ============================================================================
// PresentStage — blits the final texture to the swapchain.
// ============================================================================

class PresentStage : public IRenderStage {
  public:
	PresentStage(RGResource input, MeshHandle quad, MaterialHandle mat) : m_input(input), m_quad(quad), m_mat(mat) {
	}
	std::string_view GetName() const override {
		return "Present";
	}
	StageType GetType() const override {
		return StageType::Graphics;
	}

	void Declare(RenderGraphBuilder& b) override {
		m_present = b.ImportPresentTarget("Present");
		b.Read(m_input, ResourceUsage::Sampled, "sceneTexture");
		b.Write(m_present, ResourceUsage::ColorAttachment);
		b.SetRenderTarget(m_present);
	}
	void Compile(RenderGraphContext&) override {
	}

	void BindResources(RenderGraphContext& ctx) override {
		ctx.BindInputs(m_mat);
	}

	void Execute(RenderGraphContext& ctx) override {
		DrawRequest req{};
		req.material = m_mat;
		req.mesh = m_quad;
		ctx.GetRenderer()->DrawMesh(req);
	}

  private:
	RGResource m_input, m_present;
	MeshHandle m_quad;
	MaterialHandle m_mat;
};

// ============================================================================
// main
// ============================================================================

int main() {
	WindowVulkan window("PixieRenderer", glm::ivec2(kRenderSize));
	IRenderer* renderer = window.GetRenderer();

	// ---- Load FBX scene (materials, meshes, camera entity, script) ----
	const std::string scenePath = "/home/asuart/Repos/PixieRenderer/assets/Example.fbx";
	std::unique_ptr<Scene> scene = SceneLoader::LoadScene(scenePath, renderer);

	// Shared UBOs referenced by every PBR material in the scene.
	BufferHandle cameraUBO = renderer->CreateBuffer(BufferType::Uniform, sizeof(CameraUBO));
	BufferHandle cameraPositionUBO = renderer->CreateBuffer(BufferType::Uniform, sizeof(glm::vec4));

	// ---- Present quad (fullscreen triangle) ----
	PresentMaterial presentMat;
	MaterialHandle presentMatHandle = renderer->CreateMaterial(&presentMat);

	Mesh fsTri;
	fsTri.vertexes.resize(3);
	fsTri.vertexes[0].position = { -1.0f, -1.0f, 0.0f };
	fsTri.vertexes[1].position = { 3.0f, -1.0f, 0.0f };
	fsTri.vertexes[2].position = { -1.0f, 3.0f, 0.0f };
	fsTri.indexes = { 0, 1, 2 };
	MeshHandle fsTriHandle = renderer->CreateMesh(&fsTri);

	// ---- Blur compute program ----
	BlurComputeProgram blurProgram;
	ComputeProgramHandle blurProgramHandle = renderer->CreateComputeProgram(&blurProgram);

	// ---- Render graph ----
	RenderGraph rg(renderer);

	RenderTargetDesc rtDesc;
	rtDesc.format = TextureFormat::RGBA32f;
	rtDesc.size = kRenderSize;
	rtDesc.hasDepth = true;
	rtDesc.finalUsage = ResourceUsage::Sampled;
	RGResource sceneColor = rg.RegisterRenderTarget("SceneColor", rtDesc);

	TextureDesc blurDesc;
	blurDesc.format = TextureFormat::RGBA32f;
	blurDesc.size = kRenderSize;
	blurDesc.mipLevels = 1;
	blurDesc.storageImage = true;
	RGResource blurColor = rg.RegisterTexture("BlurColor", blurDesc);

	rg.AddStage(std::make_unique<SceneStage>(*scene, sceneColor, cameraUBO, cameraPositionUBO, kRenderSize));
	rg.AddStage(std::make_unique<BlurStage>(sceneColor, blurColor, blurProgramHandle, kRenderSize));
	rg.AddStage(std::make_unique<PresentStage>(blurColor, fsTriHandle, presentMatHandle));
	rg.Compile();

	// ========================================================================
	// UI — docking ImGui via UIVulkan + UIWindow subclasses
	// ========================================================================
	std::unique_ptr<UIVulkan> ui = std::make_unique<UIVulkan>(&window, /*docking=*/true);

	ui->AddWindow(new DemoWindow(ui.get(), renderer));
	ui->AddWindow(new ApplicationStatsWindow(ui.get(), renderer));
	ui->AddWindow(new TextureDisplayWindow(ui.get(), renderer, rg.GetResource(blurColor).texture));

	// ---- Main loop ----
	while (!window.GetShouldClose()) {
		window.PollEvents();

		if (!renderer->BeginFrame())
			continue;

		Time::Update();

		// Let each UIWindow update any offscreen resources before the frame.
		ui->OnBeforeDrawFrame();

		// Run ECS scripts (FreeCameraController updates the camera Transform).
		scene->Update();

		rg.Execute();

		// ImGui draw data is submitted via the present overlay hook, so this
		// must run after rg.Execute() and before EndFrame().
		ui->Draw();

		renderer->EndFrame();
	}

	// Tear the UI down *before* clearing graph resources so ImGui no longer
	// references textures / framebuffers the graph is about to Reset().
	ui.reset();

	renderer->WaitIdle();
	rg.Clear();

	// scene is destroyed here; it owns all shared_ptr<Mesh> and shared_ptr<IMaterial>.
	return 0;
}