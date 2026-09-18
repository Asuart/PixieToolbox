#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <PixieRenderer/Camera/Camera.h>
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

#include "PixieToolbox/UI/UI.h"
#include "PixieToolbox/UI/UIVulkan.h"
#include "PixieToolbox/UI/Windows/ApplicationStatsWindow.h"
#include "PixieToolbox/UI/Windows/DemoWindow.h"
#include "PixieToolbox/UI/Windows/TextureDisplayWindow.h"

using namespace PixieRenderer;
using namespace PixieToolbox;

static constexpr int kMaxTextureSize = 2048;
static constexpr glm::uvec2 kRenderSize = { 1280, 720 };

static std::filesystem::path gAssetRoot;

struct SceneData {
	struct CameraUBO {
		glm::mat4 view;
		glm::mat4 projection;
	};
	struct CameraState {
		glm::vec3 position = glm::vec3(0.0f, 1.5f, -4.0f);
		float yaw = -90.0f;
		float pitch = -5.0f;
		float moveSpeed = 30.0f;
		float mouseSensitivity = 0.2f;
	} cam;

	CameraUBO cameraData{};

	std::vector<std::unique_ptr<PBRMaterial>> materials;
	std::vector<MaterialHandle> materialHandles;
	std::unordered_map<std::string, TextureHandle> textureCache;
	std::vector<MeshHandle> meshStorage;

	struct DrawItem {
		MeshHandle mesh;
		MaterialHandle material;
		glm::mat4 transform;
	};
	std::vector<DrawItem> items;

	BufferHandle cameraUBO;
	BufferHandle cameraPositionUBO;
};

class SceneStage : public IRenderStage {
  public:
	SceneStage(SceneData& s, RGResource output) : m_scene(s), m_output(output) {
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
		const glm::vec4 camPos = glm::vec4(glm::vec3(glm::inverse(m_scene.cameraData.view)[3]), 1.0f);
		r->UpdateBuffer(m_scene.cameraUBO, std::as_bytes(std::span{ &m_scene.cameraData, 1 }));
		r->UpdateBuffer(m_scene.cameraPositionUBO, std::as_bytes(std::span{ &camPos, 1 }));
		for (MaterialHandle mh : m_scene.materialHandles) {
			if (!mh)
				continue;
			r->BindBuffer(mh, "CameraUBO", m_scene.cameraUBO);
			r->BindBuffer(mh, "CameraPosition", m_scene.cameraPositionUBO);
		}
		for (auto& m : m_scene.materials)
			m->Bind(r);
	}

	void Execute(RenderGraphContext& ctx) override {
		IRenderer* r = ctx.GetRenderer();
		for (const auto& item : m_scene.items) {
			DrawRequest req{};
			req.material = item.material;
			req.mesh = item.mesh;
			req.inlineData = std::as_bytes(std::span{ &item.transform, 1 });
			r->DrawMesh(req);
		}
	}

  private:
	SceneData& m_scene;
	RGResource m_output;
};

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
// Camera
// ============================================================================

static void UpdateCamera(SceneData& s, float dt, glm::uvec2 resolution) {
	auto& c = s.cam;
	const bool looking = UserInput::IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT);
	if (looking) {
		auto md = UserInput::GetMouseDelta();
		c.yaw += static_cast<float>(md.x) * c.mouseSensitivity;
		c.pitch -= static_cast<float>(md.y) * c.mouseSensitivity;
		c.pitch = glm::clamp(c.pitch, -89.0f, 89.0f);
	}
	const float yr = glm::radians(c.yaw), pr = glm::radians(c.pitch);
	const glm::vec3 forward(std::cos(pr) * std::cos(yr), std::sin(pr), std::cos(pr) * std::sin(yr));
	const glm::vec3 up(0, 1, 0);
	const glm::vec3 right = glm::normalize(glm::cross(forward, up));

	float speed = c.moveSpeed;
	if (UserInput::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		speed *= 4.0f;

	glm::vec3 vel(0);
	if (UserInput::IsKeyDown(GLFW_KEY_W))
		vel += forward;
	if (UserInput::IsKeyDown(GLFW_KEY_S))
		vel -= forward;
	if (UserInput::IsKeyDown(GLFW_KEY_D))
		vel += right;
	if (UserInput::IsKeyDown(GLFW_KEY_A))
		vel -= right;
	if (UserInput::IsKeyDown(GLFW_KEY_SPACE))
		vel += up;
	if (UserInput::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
		vel -= up;
	if (glm::length(vel) > 0.0f)
		c.position += glm::normalize(vel) * speed * dt;

	s.cameraData.view = glm::lookAt(c.position, c.position + forward, up);
	const float aspect = float(resolution.x) / float(resolution.y);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5000.0f);
	proj[1][1] *= -1.0f; // Vulkan Y-flip
	s.cameraData.projection = proj;
}

// ============================================================================
// main
// ============================================================================

using namespace PixieToolbox;

int main() {
	WindowVulkan window("PixieRenderer", glm::ivec2(kRenderSize));
	IRenderer* renderer = window.GetRenderer();

	// ---- USD scene (unchanged) ----
	const std::string scenePath = "/home/asuart/Repos/PixieRenderer/assets/main_sponza/NewSponza_Main_USD_Yup_003.usda";
	gAssetRoot = std::filesystem::path(scenePath).parent_path();

	SceneData scene;
	//LoadScene(scene, renderer, scenePath);
	scene.cameraUBO = renderer->CreateBuffer(BufferType::Uniform, sizeof(SceneData::CameraUBO));
	scene.cameraPositionUBO = renderer->CreateBuffer(BufferType::Uniform, sizeof(glm::vec4));

	// ---- Present quad (unchanged) ----
	PresentMaterial presentMat;
	MaterialHandle presentMatHandle = renderer->CreateMaterial(&presentMat);

	Mesh fsTri;
	fsTri.vertexes.resize(3);
	fsTri.vertexes[0].position = { -1.0f, -1.0f, 0.0f };
	fsTri.vertexes[1].position = { 3.0f, -1.0f, 0.0f };
	fsTri.vertexes[2].position = { -1.0f, 3.0f, 0.0f };
	fsTri.indexes = { 0, 1, 2 };
	MeshHandle fsTriHandle = renderer->CreateMesh(&fsTri);

	// ---- Blur (unchanged) ----
	BlurComputeProgram blurProgram;
	ComputeProgramHandle blurProgramHandle = renderer->CreateComputeProgram(&blurProgram);

	// ---- Render graph (unchanged) ----
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

	rg.AddStage(std::make_unique<SceneStage>(scene, sceneColor));
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

		// Let each UIWindow update any offscreen resources (e.g.
		// TextureDisplayWindow may re-render into its own FBO).
		ui->OnBeforeDrawFrame();

		UpdateCamera(scene, Time::deltaTime, window.GetResolution());
		rg.Execute();

		// ImGui draw data is submitted by UIVulkan through the present
		// overlay hook, so this must be called after rg.Execute() and
		// before EndFrame() while the present pass is alive.
		ui->Draw();

		renderer->EndFrame();
	}

	// Tear the UI down *before* clearing graph resources so ImGui no longer
	// references textures / framebuffers the graph is about to Reset().
	ui.reset();

	renderer->WaitIdle();
	rg.Clear();
	return 0;
}
